# React

React 是 Facebook（現 Meta）於 2013 年發布的 JavaScript UI 庫，徹底改變了前端開發的方式。React 採用元件化開發思維，引入虛擬 DOM 與單向資料流概念，讓開發者能夠構建大型、複雜的單頁應用程式。React 的核心理念是「宣告式設計」，開發者只需描述 UI 應該呈現的樣子，React 會自動處理更新邏輯。這種方式大大降低了程式碼的複雜度，提高了可維護性，也為後續的 Vue、Angular 等框架奠定了基礎。

## React 核心概念

### 虛擬 DOM

React 使用虛擬 DOM 來提升效能：

```
┌─────────────────────────────────────────────────────────────────┐
│                      虛擬 DOM                                  │
├─────────────────────────────────────────────────────────────────┤
│                                                              │
│  傳統 DOM 更新：                                              │
│  DOM 樹 → 解析 → 渲染 → 重新渲染 (昂貴)                        │
│                                                              │
│  虛擬 DOM 更新：                                             │
│  JS 物件 → 差異計算 → 最小更新 → 實際 DOM (快速)             │
│                                                              │
│  優勢：                                                       │
│  • 減少實際 DOM 操作                                          │
│  • 跨平台渲染                                                 │
│  • 可預測的更新邏輯                                           │
│                                                              │
└─────────────────────────────────────────────────────────────────┘
```

### 宣告式 vs 命令式

```jsx
// 宣告式：描述結果
function Counter() {
    const [count, setCount] = useState(0);
    return <button onClick={() => setCount(count + 1)}>{count}</button>;
}

// 命令式：描述步驟
// const btn = document.getElementById('btn');
// btn.addEventListener('click', () => {
//     const current = parseInt(btn.innerText);
//     btn.innerText = current + 1;
// });
```

## JSX 語法

JSX 是 JavaScript 的語法擴展，讓你能在 JavaScript 中寫 HTML 類似的語法：

```jsx
// 基本語法
const element = <h1 className="title">Hello World</h1>;

// 嵌入表達式
const name = "John";
const greeting = <p>Hello, {name}!</p>;

// 條件渲染
const showMessage = true;
const content = (
    <div>
        {showMessage && <p>Message here</p>}
        {showMessage ? <p>Yes</p> : <p>No</p>}
    </div>
);

// 列表渲染
const items = ['Apple', 'Banana', 'Orange'];
const list = (
    <ul>
        {items.map(item => <li key={item}>{item}</li>)}
    </ul>
);

// 事件處理
function handleClick() {
    console.log('Clicked!');
}
<button onClick={handleClick}>Click me</button>
```

## 元件

### 函式元件

```jsx
// 函式元件 (推薦)
function Welcome({ name, age = 18 }) {
    return <h1>Hello, {name}! You are {age} years old.</h1>;
}

// 使用
<Welcome name="John" age={25} />
<Welcome name="Jane" />  // age 為預設值 18
```

### 類別元件

```jsx
// 類別元件 (舊式，仍支援)
class Welcome extends React.Component {
    render() {
        return <h1>Hello, {this.props.name}!</h1>;
    }
}
```

### 元件組合

```jsx
// 組合範例
function App() {
    return (
        <div className="app">
            <Header title="My App" />
            <main>
                <Sidebar />
                <Content />
            </main>
            <Footer />
        </div>
    );
}

function Header({ title }) {
    return <header><h1>{title}</h1></header>;
}
```

## 狀態管理

### useState Hook

```jsx
import { useState } from 'react';

// 基本使用
function Counter() {
    const [count, setCount] = useState(0);
    
    return (
        <div>
            <p>Count: {count}</p>
            <button onClick={() => setCount(count + 1)}>Increment</button>
            <button onClick={() => setCount(c => c - 1)}>Decrement</button>
            <button onClick={() => setCount(0)}>Reset</button>
        </div>
    );
}

// 多個狀態
function Form() {
    const [name, setName] = useState('');
    const [email, setEmail] = useState('');
    const [age, setAge] = useState(0);
    
    return (
        <form>
            <input value={name} onChange={e => setName(e.target.value)} />
            <input value={email} onChange={e => setEmail(e.target.value)} />
            <input type="number" value={age} onChange={e => setAge(e.target.value)} />
        </form>
    );
}
```

### useReducer Hook

```jsx
import { useReducer } from 'react';

const initialState = { count: 0 };

function reducer(state, action) {
    switch (action.type) {
        case 'increment':
            return { count: state.count + 1 };
        case 'decrement':
            return { count: state.count - 1 };
        case 'reset':
            return { count: 0 };
        default:
            return state;
    }
}

function Counter() {
    const [state, dispatch] = useReducer(reducer, initialState);
    
    return (
        <>
            <p>Count: {state.count}</p>
            <button onClick={() => dispatch({ type: 'increment' })}>+</button>
            <button onClick={() => dispatch({ type: 'decrement' })}>-</button>
            <button onClick={() => dispatch({ type: 'reset' })}>Reset</button>
        </>
    );
}
```

## Hooks 系統

### useEffect Hook

```jsx
import { useState, useEffect } from 'react';

// 基本用法：元件載入後執行
function DataFetcher() {
    const [data, setData] = useState(null);
    const [loading, setLoading] = useState(true);
    
    useEffect(() => {
        fetch('/api/data')
            .then(res => res.json())
            .then(data => {
                setData(data);
                setLoading(false);
            });
    }, []);  // 空依賴陣列 = 只執行一次
    
    if (loading) return <div>Loading...</div>;
    return <div>{data}</div>;
}

// 清理函式
function Timer() {
    const [seconds, setSeconds] = useState(0);
    
    useEffect(() => {
        const interval = setInterval(() => {
            setSeconds(s => s + 1);
        }, 1000);
        
        // 清理：元件卸載時清除
        return () => clearInterval(interval);
    }, []);
    
    return <div>Seconds: {seconds}</div>;
}

// 依賴變化時執行
function Search({ query }) {
    const [results, setResults] = useState([]);
    
    useEffect(() => {
        // 當 query 改變時重新搜尋
        searchApi(query).then(setResults);
    }, [query]);  // 依賴 query
    
    return <div>{results.map(r => <div key={r.id}>{r.name}</div>)}</div>;
}
```

### useContext Hook

```jsx
// 建立 Context
import { createContext, useContext } from 'react';

const ThemeContext = createContext('light');

function App() {
    return (
        <ThemeContext.Provider value="dark">
            <Toolbar />
        </ThemeContext.Provider>
    );
}

function Toolbar() {
    return <ThemedButton />;
}

function ThemedButton() {
    const theme = useContext(ThemeContext);
    return <button className={theme}>I'm styled</button>;
}
```

### 自訂 Hook

```jsx
// 自訂 Hook 範例
function useLocalStorage(key, initialValue) {
    const [value, setValue] = useState(() => {
        const stored = localStorage.getItem(key);
        return stored ? JSON.parse(stored) : initialValue;
    });
    
    useEffect(() => {
        localStorage.setItem(key, JSON.stringify(value));
    }, [key, value]);
    
    return [value, setValue];
}

// 使用
function App() {
    const [name, setName] = useLocalStorage('name', '');
    return <input value={name} onChange={e => setName(e.target.value)} />;
}

// 另一個範例：useFetch
function useFetch(url) {
    const [data, setData] = useState(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);
    
    useEffect(() => {
        fetch(url)
            .then(res => res.json())
            .then(setData)
            .catch(err => setError(err))
            .finally(() => setLoading(false));
    }, [url]);
    
    return { data, loading, error };
}
```

## 生命週期對應

### 元件生命週期

```jsx
// 對應類別元件的生命週期

// componentDidMount - 首次渲染後
useEffect(() => {
    console.log('Component mounted');
}, []);

// componentDidUpdate - 每次更新後
useEffect(() => {
    console.log('Component updated');
}); // 無依賴 = 每次渲染後

// componentWillUnmount - 卸載前
useEffect(() => {
    return () => {
        console.log('Component will unmount');
    };
}, []);

// componentDidCatch - 錯誤處理
class ErrorBoundary extends React.Component {
    componentDidCatch(error, info) {
        console.error('Error:', error, info);
    }
    render() {
        return this.props.children;
    }
}
```

## 表單處理

### 受控元件

```jsx
function Form() {
    const [formData, setFormData] = useState({
        username: '',
        email: '',
        password: ''
    });
    
    const handleSubmit = (e) => {
        e.preventDefault();
        console.log(formData);
    };
    
    const handleChange = (e) => {
        const { name, value } = e.target;
        setFormData(prev => ({ ...prev, [name]: value }));
    };
    
    return (
        <form onSubmit={handleSubmit}>
            <input
                name="username"
                value={formData.username}
                onChange={handleChange}
            />
            <input
                name="email"
                type="email"
                value={formData.email}
                onChange={handleChange}
            />
            <input
                name="password"
                type="password"
                value={formData.password}
                onChange={handleChange}
            />
            <button type="submit">Submit</button>
        </form>
    );
}
```

## 效能優化

### React.memo

```jsx
// 只有 props 改變才重新渲染
const MemoizedComponent = React.memo(function MyComponent({ data }) {
    return <div>{data.value}</div>;
});

// 自訂比較函式
const MemoizedWithCompare = React.memo(function MyComponent({ data }) {
    return <div>{data.value}</div>;
}, (prevProps, nextProps) => {
    return prevProps.data.id === nextProps.data.id;
});
```

### useMemo 與 useCallback

```jsx
function ExpensiveComponent({ a, b }) {
    // 記憶昂貴計算結果
    const result = useMemo(() => {
        return computeExpensiveValue(a, b);
    }, [a, b]);
    
    // 記憶函式（避免子元件不必要渲染）
    const handleClick = useCallback(() => {
        console.log(a, b);
    }, [a, b]);
    
    return <button onClick={handleClick}>{result}</button>;
}
```

## 生態系統

### 主要函式庫

```
React 生態：
• Next.js - SSR/SSG 框架
• Remix - 全端框架
• Redux - 狀態管理
• Zustand - 輕量狀態管理
• React Router - 路由
• React Native - 行動應用
• React Query - 伺服器狀態
• React Hook Form - 表單處理
• Styled Components - CSS-in-JS
• Tailwind CSS - 工具類 CSS
```

### Next.js 整合

```jsx
// Next.js 中的 React
// app/page.js (App Router)
export default function Page() {
    return <h1>Welcome to Next.js!</h1>;
}

// 或使用 client component
'use client';
import { useState } from 'react';

export default function Counter() {
    const [count, setCount] = useState(0);
    return <button onClick={() => setCount(c => c + 1)}>{count}</button>;
}
```

## 現代 React 特性

### Concurrent Mode

```jsx
// Suspense 和 Concurrent
import { Suspense } from 'react';

function App() {
    return (
        <Suspense fallback={<Loading />}>
            <DataComponent />
        </Suspense>
    );
}

function DataComponent() {
    // 這個元件可以使用 startTransition
    const [value, setValue] = useState('');
    
    const handleChange = (e) => {
        // 標記為非緊急更新
        startTransition(() => {
            setValue(e.target.value);
        });
    };
}
```

## 相關概念

- [前端框架](前端框架.md) - 前端框架總覽
- [JavaScript](JavaScript.md) - JavaScript 語言
- [Next.js](Next.js.md) - Next.js 框架
- [React Native](React_Native.md) - 行動應用開發
- [TypeScript](TypeScript.md) - TypeScript 支援

## 範例代碼

- [01-basics.html](../_code/網頁設計/React/01-basics.html) - React 基本範例 (Hooks, State, Effect, Context, Form, Memo)