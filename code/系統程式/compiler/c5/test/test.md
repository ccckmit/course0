cd /home/claude
echo "=== extern 變數測試 ==="
cat > /tmp/lib_counter.c << 'EOF'
int counter = 0;
int get_counter() { return counter; }
int inc_counter() { counter = counter + 1; return counter; }
EOF

cat > /tmp/main_counter.c << 'EOF'
extern int counter;
extern int get_counter();
extern int inc_counter();
int main() {
  printf("counter=%d\n", get_counter());
  inc_counter();
  inc_counter();
  printf("counter=%d\n", get_counter());
  printf("direct=%d\n", counter);
  return 0;
}
EOF

./c4 -o /tmp/lib_counter.elf /tmp/lib_counter.c
echo "lib_counter: $?"
./c4 -o /tmp/main_counter.elf /tmp/main_counter.c
echo "main_counter: $?"

python3 << 'EOF'
import struct
def r8(b,p): return b[p]
def r16(b,p): return struct.unpack_from('<H',b,p)[0]
def r32(b,p): return struct.unpack_from('<I',b,p)[0]
def r64(b,p): return struct.unpack_from('<Q',b,p)[0]

with open('/tmp/main_counter.elf','rb') as f: buf=f.read()
e_shoff=r64(buf,40); e_shstrndx=r16(buf,62)
secs=[]
for i in range(r16(buf,60)):
    b2=e_shoff+i*64
    secs.append({'idx':i,'no':r32(buf,b2),'type':r32(buf,b2+4),
                 'off':r64(buf,b2+24),'size':r64(buf,b2+32),'link':r32(buf,b2+40),'entsz':r64(buf,b2+56)})
shd=buf[secs[e_shstrndx]['off']:secs[e_shstrndx]['off']+secs[e_shstrndx]['size']]
sn=lambda n: shd[n:shd.index(b'\x00',n)].decode()
sym_sec=next(s for s in secs if sn(s['no'])=='.symtab')
str_sec=secs[sym_sec['link']]
strtab=buf[str_sec['off']:str_sec['off']+str_sec['size']]
yn=lambda n: strtab[n:strtab.index(b'\x00',n)].decode()

print("\nmain_counter.elf symtab:")
for i in range(sym_sec['size']//24):
    b2=sym_sec['off']+i*24
    nm=yn(r32(buf,b2)); info=r8(buf,b2+4); shndx=r16(buf,b2+6); val=r64(buf,b2+8)
    print(f"  [{i}] {nm:<16} info={info:#04x} {'UNDEF' if shndx==0 else f'sec{shndx}':<8} val={val}")

rs=next(s for s in secs if sn(s['no'])=='.rela')
print(f"\nmain_counter.elf rela ({rs['size']//24} entries):")
for i in range(rs['size']//24):
    b2=rs['off']+i*24
    ro=r64(buf,b2); ri=r64(buf,b2+8); rt=r64(buf,b2+16)
    sname=yn(r32(buf,sym_sec['off']+ri*24))
    tn='RELA_DATA' if rt==1 else 'RELA_FUNC'
    print(f"  [{i}] slot={ro:3d}  sym[{ri}]={sname:<14}  {tn}")
EOF