# TypeScript

TypeScript 是微軟開發的開源程式語言，是 JavaScript 的超集，添加了靜態型別定義。TypeScript 提高了大型專案的程式碼品質和可維護性，同時支援完整的 ECMAScript 標準。

## TypeScript 的優勢

### 靜態型別

```typescript
// 類型註解
let name: string = "Alice";
let age: number = 30;
let isStudent: boolean = false;

// 陣列
let numbers: number[] = [1, 2, 3];
let names: Array<string> = ["Alice", "Bob"];

// 函式
function greet(name: string): string {
    return `Hello, ${name}!`;
}

// 物件
interface User {
    name: string;
    age: number;
    email?: string;  // 可選屬性
}

const user: User = {
    name: "Alice",
    age: 30
};
```

### 編譯時錯誤檢測

```typescript
// TypeScript 在編譯時就能發現錯誤
function processUser(user: User) {
    console.log(user.name.toUpperCase());  // OK
    console.log(user.height);  // 編譯錯誤！User 沒有 height 屬性
}
```

## 基本類型

### 原始類型

```typescript
// string
let firstName: string = "John";

// number（沒有區分 int 和 float）
let count: number = 42;
let price: number = 3.14;

// boolean
let isActive: boolean = true;

// null 和 undefined
let n: null = null;
let u: undefined = undefined;

// symbol
let sym: symbol = Symbol("id");

// bigint
let big: bigint = 100n;
```

### 特殊類型

```typescript
// any（任意類型，關閉型別檢查）
let data: any = getData();
data.anything();  // 編譯通過，運行時可能錯誤

// unknown（類型安全 any）
let unknownValue: unknown = getValue();
// unknownValue.anything();  // 編譯錯誤
if (typeof unknownValue === "string") {
    console.log(unknownValue.toUpperCase());  // 安全
}

// never（永不返回）
function throwError(message: string): never {
    throw new Error(message);
}

function infiniteLoop(): never {
    while (true) {}
}

// void（無返回值）
function logMessage(message: string): void {
    console.log(message);
}
```

## 介面與類型

### 介面

```typescript
interface Point {
    x: number;
    y: number;
}

interface LabeledPoint extends Point {
    label: string;
}

const labeledPoint: LabeledPoint = {
    x: 10,
    y: 20,
    label: "origin"
};
```

### 類型別名

```typescript
type ID = string | number;
type Callback = (data: string) => void;

type UserOrAdmin = User | Admin;

// 交叉類型
type Employee = User & { employeeId: string };

// 映射類型
type Readonly<T> = {
    readonly [P in keyof T]: T[P];
};

type Partial<T> = {
    [P in keyof T]?: T[P];
};
```

### 類

```typescript
class Animal {
    private name: string;  // 私有
    protected age: number;  // 保護，子類可見
    public species: string;  // 公有
    
    constructor(name: string, age: number) {
        this.name = name;
        this.age = age;
        this.species = "Unknown";
    }
    
    move(distance: number): void {
        console.log(`${this.name} moved ${distance}m`);
    }
}

class Dog extends Animal {
    breed: string;
    
    constructor(name: string, age: number, breed: string) {
        super(name, age);
        this.breed = breed;
    }
    
    override move(distance: number): void {
        console.log(`${this.name} ran ${distance}m`);
    }
    
    bark(): void {
        console.log("Woof!");
    }
}
```

## 泛型

### 基本泛型

```typescript
function identity<T>(arg: T): T {
    return arg;
}

let output = identity<string>("hello");  // 明確類型
let output2 = identity(42);  // 自動推斷

// 泛型介面
interface ApiResponse<T> {
    data: T;
    status: number;
    message: string;
}

// 泛型類
class Queue<T> {
    private items: T[] = [];
    
    enqueue(item: T): void {
        this.items.push(item);
    }
    
    dequeue(): T | undefined {
        return this.items.shift();
    }
}
```

### 約束

```typescript
interface HasLength {
    length: number;
}

function logLength<T extends HasLength>(arg: T): number {
    return arg.length;
}

logLength("hello");      // OK
logLength([1, 2, 3]);    // OK
logLength({ length: 10 }); // OK
// logLength(123);       // 錯誤！number 沒有 length

// keyof 約束
function getProperty<T, K extends keyof T>(obj: T, key: K): T[K] {
    return obj[key];
}
```

## 類型守卫

```typescript
// typeof 守卫
function padLeft(value: string | number, padding: string | number) {
    if (typeof padding === "number") {
        return Array(padding + 1).join(" ") + value;
    }
    if (typeof padding === "string") {
        return padding + value;
    }
}

// instanceof 守卫
class Fish {
    swim() { console.log("Swimming"); }
}

class Bird {
    fly() { console.log("Flying"); }
}

function move(animal: Fish | Bird) {
    if (animal instanceof Fish) {
        (animal as Fish).swim();
    } else {
        (animal as Bird).fly();
    }
}

// 自定義守卫
interface Cat {
    meow(): void;
}

function isCat(animal: unknown): animal is Cat {
    return (animal as Cat).meow !== undefined;
}
```

## 枚舉

```typescript
// 數字枚舉
enum Direction {
    Up = 1,
    Down,
    Left,
    Right
}

console.log(Direction.Up);  // 1
console.log(Direction[1]);  // "Up"

// 字串枚舉
enum Status {
    Pending = "PENDING",
    Active = "ACTIVE",
    Completed = "COMPLETED"
}

// 常數枚舉（編譯時內聯）
const enum Color {
    Red = "RED",
    Green = "GREEN",
    Blue = "BLUE"
}

// 異構枚舉（混合）
enum BooleanLike {
    No = 0,
    Yes = "YES"
}
```

## 合併命名空間

```typescript
// 宣告合併
interface Animal {
    name: string;
}

interface Animal {
    age: number;
}
// 合併後：Animal { name: string; age: number }

// 命名空間
namespace Validation {
    export interface StringValidator {
        isAcceptable(s: string): boolean;
    }
    
    const lettersRegexp = /^[A-Za-z]+$/;
    
    export class LettersOnlyValidator implements StringValidator {
        isAcceptable(s: string): boolean {
            return lettersRegexp.test(s);
        }
    }
}
```

## 實用類型

### 內建工具類型

```typescript
// Partial<T> - 所有屬性可選
type PartialUser = Partial<User>;

// Required<T> - 所有屬性必需
type RequiredUser = Required<User>;

// Readonly<T> - 所有屬性唯讀
type ReadonlyUser = Readonly<User>;

// Pick<T, K> - 選擇屬性
type UserPreview = Pick<User, "name" | "email">;

// Omit<T, K> - 排除屬性
type UserWithoutPassword = Omit<User, "password">;

// Record<K, V> - 鍵值對
type UserMap = Record<string, User>;

// Exclude<T, U> - 排除類型
type NonNull = Exclude<string | null | undefined, null | undefined>;

// Extract<T, U> - 提取類型
type StatusStrings = Extract<Status, "PENDING" | "COMPLETED">;

// ReturnType<F> - 取得函式返回類型
function createUser() { return { name: "Alice" }; }
type User = ReturnType<typeof createUser>;

// Parameters<F> - 取得函式參數類型
type UserParams = Parameters<typeof createUser>;
```

### 條件類型

```typescript
// T extends U ? X : Y
type IsString<T> = T extends string ? "yes" : "no";

type A = IsString<string>;  // "yes"
type B = IsString<number>;   // "no"

// 分發
type ToArray<T> = T extends any ? T[] : never;
type B1 = ToArray<string | number>;  // string[] | number[]

// infer
type ReturnType<T> = T extends (...args: any[]) => infer R ? R : never;
```

## 裝飾器

```typescript
// 類裝飾器
function sealed(constructor: Function) {
    Object.seal(constructor);
    Object.seal(constructor.prototype);
}

// 方法裝飾器
function logging(target: any, key: string, descriptor: PropertyDescriptor) {
    const original = descriptor.value;
    descriptor.value = function(...args: any[]) {
        console.log(`Calling ${key} with`, args);
        return original.apply(this, args);
    };
}

// 屬性裝飾器
function readonly(target: any, key: string) {
    Object.defineProperty(target, key, {
        writable: false
    });
}

// 參數裝飾器
function validate(target: any, key: string, index: number) {
    // 驗證參數
}

// 使用
class Calculator {
    @logging
    add(a: number, b: number): number {
        return a + b;
    }
    
    @readonly
    version = "1.0";
}
```

## 模組

### 匯出

```typescript
// named export
export const PI = 3.14159;
export function add(a: number, b: number): number {
    return a + b;
}

// default export
export default class Calculator {
    // ...
}

// 重新匯出
export { add as sum } from "./math";
```

### 匯入

```typescript
// named import
import { add, PI } from "./math";

// namespace import
import * as math from "./math";

// type import（只匯入類型，不影響運行時）
import type { User, Admin } from "./types";

// dynamic import
const module = await import("./module");
```

## 設定檔

### tsconfig.json

```json
{
    "compilerOptions": {
        "target": "ES2020",
        "module": "ESNext",
        "lib": ["ES2020", "DOM"],
        "outDir": "./dist",
        "rootDir": "./src",
        "strict": true,
        "esModuleInterop": true,
        "skipLibCheck": true,
        "forceConsistentCasingInFileNames": true,
        "moduleResolution": "node",
        "resolveJsonModule": true,
        "declaration": true,
        "declarationMap": true,
        "sourceMap": true,
        "noUnusedLocals": true,
        "noUnusedParameters": true,
        "noImplicitReturns": true,
        "noFallthroughCasesInSwitch": true
    },
    "include": ["src/**/*"],
    "exclude": ["node_modules", "dist"]
}
```

## 與 JavaScript 的差異

| 功能 | TypeScript | JavaScript |
|------|-----------|------------|
| 型別系統 | 靜態、可選 | 動態 |
| 編譯 | 需要 | 不需要 |
| 介面 | 有 | 無 |
| 泛型 | 有 | 無 |
| 命名空間 | 有 | 無 |
| 元組 | 有 | 無（陣列） |
| 裝飾器 | 有（實驗性） | 無 |

## 應用場景

- **大型 Web 應用**：React、Angular、Vue 3
- **Node.js 後端**：Express、NestJS
- **行動應用**：React Native、NativeScript
- **桌面應用**：Electron、TAURI

## 相關主題

- [JavaScript](JavaScript.md) - TypeScript 的基礎
- [React](React.md) - 常與 TypeScript 配合使用
- [Node.js](Node.js.md) - 後端 TypeScript
