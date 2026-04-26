# X.509 憑證解析深入解析

## 1. 演算法概述

X.509 是 ITU-T 定義的公鑰憑證標準，用於綁定公鑰與實體身份。廣泛用於 HTTPS、TLS、程式碼簽章等場景。

## 2. X.509 憑證結構

### ASN.1（Abstract Syntax Notation One）

ASN.1 是描述資料結構的語法標準：

- **標籤（Tag）**：類型識別
- **長度（Length）**：資料長度
- **內容（Content）**：實際資料

### 基本類型

| 類型 | 標籤 | 說明 |
|------|------|------|
| INTEGER | 0x02 | 整數 |
| BIT STRING | 0x03 | 位元串 |
| OCTET STRING | 0x04 | 位元組串 |
| NULL | 0x05 | 空 |
| OBJECT IDENTIFIER | 0x06 | 物件識別碼 |
| PrintableString | 0x13 | 可列印字串 |
| T61String | 0x14 | T61 字串 |
| IA5String | 0x16 | IA5 字串（ASCII） |
| UTCTime | 0x17 | UTC 時間 |
| GeneralizedTime | 0x18 | 通用時間 |
| SEQUENCE | 0x30 | 序列 |
| SET | 0x31 | 集合 |

### 長度編碼

- 若 < 128：1 位元組，高位 0
- 若 ≥ 128：多 位元組，第一位元組 & 0x7F 表示後續位元組數

### BER vs DER

- **BER**（Basic Encoding Rules）：可變長度，可有多種表示
- **DER**（Distinguished Encoding Rules）：唯一表示，適合簽章

## 3. PEM 格式

### 結構

```
-----BEGIN CERTIFICATE-----
[Base64 編碼的 DER]
-----END CERTIFICATE-----
```

### Base64

將 3 位元組 → 4 字元：
- 24 位元（3 位元組）分為 4 × 6 位元群組
- 查表（A-Z, a-z, 0-9, +, /）
- = 填充

### 解碼過程

```c
static int base64_decode(const char *src, size_t src_len, uint8_t *out, size_t *out_len) {
    // ...
    buffer = (buffer << 6) | val;
    bits += 6;
    if (bits >= 8) {
        bits -= 8;
        out[j++] = (buffer >> bits) & 0xFF;
    }
    // ...
}
```

## 4. X.509 憑證語法

### ASN.1 定义

```
Certificate ::= SEQUENCE {
    tbsCertificate       TBSCertificate,
    signatureAlgorithm   AlgorithmIdentifier,
    signatureValue      BIT STRING
}

TBSCertificate ::= SEQUENCE {
    version         [0]  Version DEFAULT v1,
    serialNumber         CertificateSerialNumber,
    signature           AlgorithmIdentifier,
    issuer              Name,
    validity           Validity,
    subject            Name,
    subjectPublicKeyInfo SubjectPublicKeyInfo,
    ...
}
```

### 關鍵欄位

1. **TBSCertificate**：要簽章的部分
2. **signatureAlgorithm**：簽章演算法（如 SHA-256 with RSA）
3. **signatureValue**：簽章值

### Name 結構

```
Name ::= SEQUENCE OF RelativeDistinguishedName
RelativeDistinguishedName ::= SET OF AttributeTypeAndValue
AttributeTypeAndValue ::= SEQUENCE {
    type    AttributeType,
    value   AttributeValue
}
```

常見類型：
- 2.5.4.6： countryName（C）
- 2.5.4.10： organizationName（O）
- 2.5.4.3： commonName（CN）

## 5. 本實作分析

### x509_parse_from_pem

```c
int x509_parse_from_pem(const char *pem, x509_cert *cert) {
    // 1. 解析 PEM 標頭
    // 2. Base64 解碼
    // 3. DER 解析
    // 4. 提取公鑰
}
```

### 步驟分解

#### 步驟 1：PEM 解析

```c
const char *begin = strstr(pem, "-----BEGIN");
const char *end = strstr(begin, "-----END");
size_t b64_len = end - begin_nl;
```

#### 步驟 2：Base64 解碼

```c
base64_decode(b64_clean, clean_len, der, &der_len);
```

#### 步驟 3：DER 解析

```c
// 檢查 SEQUENCE 標籤
if (der_len < 4 || der[0] != 0x30) return -1;

// 解析長度
get_asn1_length(der, der_len, &cert_len, &cert_header);
```

#### 步驟 4：提取公鑰

```c
// TBSCertificate
const uint8_t *tbs = der + cert_header;
get_asn1_length(tbs, cert_len, &tbs_len, &tbs_header);

// 遍历 TBSCertificate 尋找 SubjectPublicKeyInfo
while (pos + 4 < tbs_content_len && !found_key) {
    // 檢查每個 SEQUENCE
    if (tag == 0x30) {
        // 解析 AlgorithmIdentifier
        // 解析 SubjectPublicKeyInfo
        // 解析 RSA 公鑰：n, e
    }
}
```

### 公鑰提取

```c
// RSA 公鑰結構
RSAPublicKey ::= SEQUENCE {
// n: modulus
// e: publicExponent
}

// 提取 n
memcpy(cert->public_key.n, n_data, n_copy_len);
cert->public_key.n_len = n_copy_len;

// 提取 e
memcpy(cert->public_key.e, e_value, e_len);
cert->public_key.e_len = e_len;
```

## 6. X.509 擴展

### 常見擴展

- **Basic Constraints**：ca=TRUE/FALSE（是否為 CA 憑證）
- **Key Usage**：金鑰用途（數位簽章、金鑰加密等）
- **Subject Alternative Name**：主體別名（DNS 名稱、IP 位址）
- **CRL Distribution Points**： CRL 分發點

### 憑證鍊

```
Root CA
  ├── Intermediate CA 1
  │     └── Intermediate CA 2
  │           └── End-Entity Certificate
```

驗證：
1. 檢查簽章者
2. 檢查有效期
3. 檢查吊銷（CRL/OCSP）
4. 檢查名稱约束

## 7. 安全性考量

### 本實作限制

- 僅提取公鑰（n, e）
- 未驗證簽章
- 未檢查有效期
- 未處理擴展
- 未驗證名稱约束

### 應有的驗證

```c
int verify_certificate(x509_cert *cert, x509_cert *issuer) {
    // 1. 檢查版本
    // 2. 檢查簽章演算法
    // 3. 驗證簽章
    // 4. 檢查有效期
    // 5. 檢查主體/頒發者
    // 6. 檢查金鑰用途
    // 7. 檢查 Basic Constraints
}
```

### 信任鍊

作業系統/瀏覽器維護信任根憑證庫。

## 8. 標準演變

### X.509 v1（1988）

基本欄位，無擴展。

### X.509 v2（1993）

增加 version 欄位。

### X.509 v3（1995）

增加擴展支援。

### PKIX

IETF X.509 實現檔案：
- RFC 5280：X.509 PKI 配置文件
- RFC 6818：CA 證書運作

## 9. 使用場景

### HTTPS

```
ClientHello →（TLS 1.2 握手）
ServerHello →
Certificate →
ServerKeyExchange →
CertificateRequest →
ServerHelloDone

Certificate →
ClientKeyExchange →
CertificateVerify →
```

### 用戶端驗證

PKCS#12 用戶端憑證：
- 包含公鑰和私鑰
- 密碼保護

### 代碼簽章

- Authenticode（Windows）
- Apple Developer ID

## 10. 實踐建議

### 提取公鑰

```c
x509_cert cert;
if (x509_parse_from_pem(pem_data, &cert) != 0) {
    // 錯誤
}
uint8_t n[256], e[4];
size_t n_len, e_len;
x509_get_public_key(&cert, n, &n_len, e, &e_len);

// 使用 n, e
```

### 驗證證書

- 使用 OpenSSL 或 mbedTLS
- 或作業系統 API

### 建議

不要自己實現憑證驗證，使用已知函式庫。