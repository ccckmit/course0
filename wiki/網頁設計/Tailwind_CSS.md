# Tailwind CSS

Tailwind CSS 是一個實用優先（utility-first）的 CSS 框架，提供大量低階的樣式類別，讓開發者能快速構建自訂設計，無需離開 HTML 撰寫自定義 CSS。

## 核心概念

### 實用優先設計

傳統 CSS：
```css
.button {
    background-color: blue;
    color: white;
    padding: 0.75rem 1.5rem;
    border-radius: 0.25rem;
    font-weight: bold;
}
```

Tailwind CSS：
```html
<button class="bg-blue-500 text-white px-6 py-3 rounded font-bold">
    按鈕
</button>
```

### 無需離開 HTML

```html
<!-- 所有樣式都在 HTML 中 -->
<div class="flex items-center justify-between p-4 bg-gray-100 rounded-lg shadow">
    <h1 class="text-2xl font-bold text-gray-800">標題</h1>
    <button class="bg-blue-500 hover:bg-blue-600 text-white px-4 py-2 rounded">
        點擊
    </button>
</div>
```

## 響應式設計

### 前綴語法

```html
<!-- 行動裝置優先 -->
<div class="text-base md:text-lg lg:text-xl">
    響應式文字大小
</div>

<!-- 前綴對照 -->
<!-- 無前綴：小於 sm (640px) -->
<!-- sm：640px 及以上 -->
<!-- md：768px 及以上 -->
<!-- lg：1024px 及以上 -->
<!-- xl：1280px 及以上 -->
<!-- 2xl：1536px 及以上 -->
```

### 響應式範例

```html
<!-- 響應式佈局 -->
<div class="flex flex-col md:flex-row">
    <div class="w-full md:w-1/3 p-4">側邊欄</div>
    <div class="w-full md:w-2/3 p-4">主內容</div>
</div>

<!-- 響應式顯示 -->
<div class="hidden md:block lg:hidden">
    僅在中型螢幕顯示
</div>

<!-- 響應式間距 -->
<div class="p-4 md:p-8 lg:p-12">
    響應式內距
</div>
```

## 偽類和偽元素

### 互動狀態

```html
<!-- Hover -->
<button class="bg-blue-500 hover:bg-blue-600 ...">
    Hover 效果
</button>

<!-- Focus -->
<input class="border-2 border-gray-300 focus:border-blue-500 focus:outline-none ...">

<!-- Active -->
<button class="bg-blue-500 active:bg-blue-700 ...">
    點擊效果
</button>

<!-- Disabled -->
<button class="bg-gray-300 text-gray-500 cursor-not-allowed ..." disabled>
    禁用狀態
</button>
```

### 偽元素

```html
<!-- 群組 Hover -->
<div class="group">
    <div class="group-hover:bg-blue-500">群組 hover</div>
</div>

<!-- 第一個/最後一個 -->
<ul>
    <li class="first:text-red-500 last:text-green-500">列表項目</li>
</ul>

<!-- 奇數/偶數 -->
<tr class="odd:bg-gray-100 even:bg-white">表格列</tr>

<!-- 空元素 -->
<div class="empty:hidden">有空內容才顯示</div>
```

## 間距和尺寸

### 內距和外距

```html
<!-- 外距 -->
<div class="m-4">m-4 (1rem)</div>
<div class="mx-auto">mx-auto (水平居中)</div>
<div class="mt-8">mt-8 (上邊距 2rem)</div>
<div class="mb-4">mb-4 (下邊距 1rem)</div>
<div class="ml-2 mr-2">ml-2 mr-2</div>

<!-- 內距 -->
<div class="p-4">p-4 (padding 1rem)</div>
<div class="px-4 py-2">px-4 py-2 (水平/垂直)</div>

<!-- 間距刻度 (4px 為基數) -->
<!-- 0=0px, 1=0.25rem, 2=0.5rem, 3=0.75rem, 4=1rem -->
<!-- ... -->
<!-- 64=16rem -->
```

### 寬度和高度

```html
<!-- 寬度 -->
<div class="w-1/2">w-1/2 (50%)</div>
<div class="w-full">w-full (100%)</div>
<div class="w-screen">w-screen (100vw)</div>
<div class="w-auto">w-auto</div>

<!-- 高度 -->
<div class="h-64">h-64 (16rem)</div>
<div class="h-screen">h-screen (100vh)</div>
<div class="min-h-screen">min-h-screen</div>

<!-- 最大寬度 -->
<div class="max-w-md">max-w-md (28rem)</div>
<div class="max-w-screen-lg">max-w-screen-lg</div>
```

## 顏色

### 內建顏色

```html
<!-- 文字顏色 -->
<p class="text-red-500">紅色文字</p>
<p class="text-gray-700">灰色文字</p>
<p class="text-transparent">透明文字</p>

<!-- 背景顏色 -->
<div class="bg-blue-500">藍色背景</div>
<div class="bg-gray-100">淺灰背景</div>

<!-- 邊框顏色 -->
<input class="border-2 border-blue-300">

<!-- 漸層 -->
<div class="bg-gradient-to-r from-blue-500 to-green-500">
    漸層背景
</div>

<!-- 不透明度 -->
<div class="bg-blue-500/50">50% 透明度</div>
<div class="text-red-500/30">30% 透明度</div>
```

### 自訂顏色

```javascript
// tailwind.config.js
module.exports = {
    theme: {
        extend: {
            colors: {
                brand: {
                    50: '#f0f9ff',
                    100: '#e0f2fe',
                    500: '#0ea5e9',
                    900: '#0c4a6e',
                }
            }
        }
    }
}
```

## 文字排版

### 文字大小

```html
<p class="text-xs">extra small</p>
<p class="text-sm">small (0.875rem)</p>
<p class="text-base">base (1rem)</p>
<p class="text-lg">large (1.125rem)</p>
<p class="text-2xl">2xl (1.5rem)</p>
<p class="text-4xl">4xl (2.25rem)</p>
<p class="text-9xl">9xl (8rem)</p>
```

### 文字樣式

```html
<!-- 粗細 -->
<p class="font-thin">thin (100)</p>
<p class="font-light">light (300)</p>
<p class="font-normal">normal (400)</p>
<p class="font-bold">bold (700)</p>
<p class="font-black">black (900)</p>

<!-- 樣式 -->
<p class="italic">斜體</p>
<p class="not-italic">非斜體</p>

<!-- 行高 -->
<p class="leading-none">leading-none (1)</p>
<p class="leading-tight">leading-tight (1.25)</p>
<p class="leading-loose">leading-loose (2)</p>

<!-- 字間距 -->
<p class="tracking-tighter">tracking-tighter</p>
<p class="tracking-widest">tracking-widest</p>

<!-- 文字對齊 -->
<p class="text-left">左對齊</p>
<p class="text-center">居中</p>
<p class="text-right">右對齊</p>
<p class="text-justify">兩端對齊</p>
```

## 佈局

### Flexbox

```html
<!-- 基本 Flex -->
<div class="flex">
    <div>Flex 項目</div>
</div>

<!-- 方向 -->
<div class="flex flex-row">水平</div>
<div class="flex flex-col">垂直</div>

<!-- 對齊 -->
<div class="flex items-center justify-center">
    居中對齊
</div>
<div class="flex items-start justify-between">
    兩端對齊
</div>

<!-- 換行 -->
<div class="flex flex-wrap">換行</div>

<!-- 間距 -->
<div class="flex gap-4">
    <div>間距 1rem</div>
</div>
```

### Grid

```html
<!-- 基本 Grid -->
<div class="grid grid-cols-3">
    <div>三列</div>
</div>

<!-- 響應式列 -->
<div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4">
    響應式列數
</div>

<!-- 間距 -->
<div class="grid grid-cols-3 gap-4">
    <div>間距 1rem</div>
</div>

<!-- 跨越 -->
<div class="grid grid-cols-4">
    <div class="col-span-2">跨越 2 列</div>
    <div class="col-span-3">跨越 3 列</div>
</div>
```

## 元件化

### 使用 @apply

```css
/* input.css */
@tailwind base;
@tailwind components;
@tailwind utilities;

@layer components {
    .btn {
        @apply px-4 py-2 rounded font-medium transition-colors;
    }
    
    .btn-primary {
        @apply bg-blue-500 text-white hover:bg-blue-600;
    }
    
    .btn-secondary {
        @apply bg-gray-200 text-gray-800 hover:bg-gray-300;
    }
    
    .card {
        @apply bg-white rounded-lg shadow-md p-6;
    }
}
```

```html
<!-- 使用 -->
<button class="btn btn-primary">主要按鈕</button>
<button class="btn btn-secondary">次要按鈕</button>
<div class="card">卡片內容</div>
```

### 群組化樣式

```html
<!-- 群組 -->
<div class="group border rounded-lg p-4 hover:bg-gray-50">
    <h3 class="group-hover:text-blue-500">群組 Hover</h3>
    <p class="text-gray-600">內容</p>
</div>

<!-- 嵌套群組 -->
<div class="group">
    <div class="group-first:rounded-t-lg">
        <div class="group-hover:bg-gray-50">
            <!-- ... -->
        </div>
    </div>
</div>
```

## 暗色模式

### 類切換

```html
<!-- 預設 -->
<div class="bg-white dark:bg-gray-900">
    <h1 class="text-gray-900 dark:text-white">暗色模式</h1>
</div>
```

```javascript
// 切換腳本
if (localStorage.theme === 'dark' || 
    (!('theme' in localStorage) && 
     window.matchMedia('(prefers-color-scheme: dark)').matches)) {
    document.documentElement.classList.add('dark');
} else {
    document.documentElement.classList.remove('dark');
}
```

## 自訂配置

### tailwind.config.js

```javascript
/** @type {import('tailwindcss').Config} */
module.exports = {
    content: [
        "./src/**/*.{js,jsx,ts,tsx}",
        "./public/index.html",
    ],
    darkMode: 'class',  // 或 'media'
    theme: {
        extend: {
            colors: {
                primary: '#3b82f6',
                secondary: '#8b5cf6',
            },
            spacing: {
                '128': '32rem',
            },
            borderRadius: {
                'xl': '1rem',
            },
            animation: {
                'spin-slow': 'spin 3s linear infinite',
            },
        },
    },
    plugins: [
        require('@tailwindcss/forms'),
        require('@tailwindcss/line-clamp'),
    ],
}
```

## 漸層和陰影

### 漸層

```html
<!-- 方向 -->
<div class="bg-gradient-to-r">從左到右</div>
<div class="bg-gradient-to-t">從下到上</div>
<div class="bg-gradient-to-br">對角</div>

<!-- 多色漸層 -->
<div class="bg-gradient-to-r from-red-500 via-yellow-500 to-green-500">
    彩虹漸層
</div>
```

### 陰影

```html
<div class="shadow-sm">小陰影</div>
<div class="shadow">預設陰影</div>
<div class="shadow-md">中等陰影</div>
<div class="shadow-lg">大陰影</div>
<div class="shadow-xl">更大陰影</div>
<div class="shadow-2xl">2xl 陰影</div>
<div class="shadow-inner">內陰影</div>
<div class="shadow-none">無陰影</div>
```

## 過渡和動畫

### 過渡

```html
<button class="transition-colors duration-300 ease-in-out hover:bg-blue-600">
    過渡效果
</button>

<!-- 屬性 -->
<div class="transition-opacity duration-500">透明</div>
<div class="transition-transform hover:scale-105">變換</div>
<div class="transition-all duration-300">所有屬性</div>
```

### 動畫

```html
<div class="animate-spin">旋轉</div>
<div class="animate-pulse">脈衝</div>
<div class="animate-bounce">彈跳</div>
```

## 與其他框架整合

### React

```jsx
function Button({ children, variant = 'primary' }) {
    const baseStyles = 'px-4 py-2 rounded font-medium transition-colors';
    const variants = {
        primary: 'bg-blue-500 text-white hover:bg-blue-600',
        secondary: 'bg-gray-200 text-gray-800 hover:bg-gray-300',
    };
    
    return (
        <button className={`${baseStyles} ${variants[variant]}`}>
            {children}
        </button>
    );
}
```

### Vue

```vue
<template>
    <button 
        class="px-4 py-2 rounded font-medium transition-colors"
        :class="variantStyles"
    >
        <slot />
    </button>
</template>

<script setup>
const props = defineProps({
    variant: { type: String, default: 'primary' }
});

const variantStyles = computed(() => ({
    'bg-blue-500 text-white hover:bg-blue-600': props.variant === 'primary',
    'bg-gray-200 text-gray-800 hover:bg-gray-300': props.variant === 'secondary',
}));
</script>
```

## 相關主題

- [CSS](CSS.md) - 傳統 CSS
- [JavaScript](JavaScript.md) - JavaScript 基礎
- [前端框架](前端框架.md) - React/Vue 等框架
