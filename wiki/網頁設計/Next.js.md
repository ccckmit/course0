# Next.js

## 概述

Next.js 是 React 的元框架 (Meta-framework)，提供伺服器端渲染 (SSR)、靜態網站生成 (SSG)、API Routes 等功能，是現代 React 應用的首選框架。

## 特性

| 特性 | 說明 |
|------|------|
| SSR | 伺服器端渲染 |
| SSG | 靜態網站生成 |
| ISR | 增量靜態再生成 |
| API Routes | 全端功能 |
| 自動程式碼分割 | 效能優化 |
| 圖片優化 | Image 元件 |

## 安裝

```bash
# 建立新專案
npx create-next-app@latest my-app
# 或
npm create next-app@latest my-app

# 啟動開發伺服器
cd my-app
npm run dev
```

## 頁面系統

### 檔案路由 (File-based Routing)

```
app/
├── page.tsx              → /
├── about/
│   └── page.tsx          → /about
├── blog/
│   ├── page.tsx          → /blog
│   └── [slug]/
│       └── page.tsx      → /blog/:slug
└── (marketing)/
    ├── layout.tsx        → 共享佈局
    └── page.tsx          → /
```

### 動態路由

```tsx
// app/blog/[slug]/page.tsx
export default function BlogPost({
  params,
}: {
  params: { slug: string }
}) {
  return <h1>Post: {params.slug}</h1>
}
```

### 巢狀路由

```tsx
// app/dashboard/settings/page.tsx
export default function SettingsPage() {
  return <h1>Settings</h1>
}
```

## 渲染模式

### SSR (伺服器端渲染)

```tsx
// 預設行為，每次請求時渲染
async function Page() {
  const data = await fetch('https://api.example.com/data').then(r => r.json())
  return <div>{data.name}</div>
}

export default Page
```

### SSG (靜態網站生成)

```tsx
// 建置時產生靜態 HTML
export async function generateStaticParams() {
  const posts = await getPosts()
  return posts.map((post) => ({
    slug: post.slug,
  }))
}
```

### ISR (增量靜態再生成)

```tsx
export const revalidate = 60 // 每 60 秒重新驗證

async function Page() {
  const data = await fetch('https://api.example.com/data', {
    next: { revalidate: 60 },
  }).then(r => r.json())
  
  return <div>{data.content}</div>
}
```

## 客戶端 vs 伺服器元件

### 伺服器元件 (預設)

```tsx
// 伺服器元件 - 無互動
async function ServerComponent() {
  const data = await db.query('SELECT * FROM posts')
  return (
    <ul>
      {data.map(post => (
        <li key={post.id}>{post.title}</li>
      ))}
    </ul>
  )
}
```

### 客戶端元件

```tsx
'use client'

import { useState } from 'react'

function ClientComponent() {
  const [count, setCount] = useState(0)
  
  return (
    <button onClick={() => setCount(count + 1)}>
      Count: {count}
    </button>
  )
}
```

### 混合使用

```tsx
// 父元件是伺服器元件
async function Parent() {
  const data = await getData()
  
  return (
    <div>
      <ServerData data={data} />
      <ClientComponent />  {/* 客戶端互動 */}
    </div>
  )
}
```

## API Routes

```tsx
// app/api/users/route.ts
import { NextResponse } from 'next/server'

export async function GET() {
  const users = await db.query('SELECT * FROM users')
  return NextResponse.json(users)
}

export async function POST(request: Request) {
  const body = await request.json()
  const user = await db.create('users', body)
  return NextResponse.json(user, { status: 201 })
}
```

### 動態 API 路由

```tsx
// app/api/users/[id]/route.ts
export async function GET(
  request: Request,
  { params }: { params: { id: string } }
) {
  const user = await db.find('users', params.id)
  if (!user) {
    return NextResponse.json({ error: 'User not found' }, { status: 404 })
  }
  return NextResponse.json(user)
}

export async function DELETE(
  request: Request,
  { params }: { params: { id: string } }
) {
  await db.delete('users', params.id)
  return new NextResponse(null, { status: 204 })
}
```

## 樣式

### Tailwind CSS

```tsx
export default function Component() {
  return (
    <div className="p-4 bg-white rounded-lg shadow">
      <h1 className="text-2xl font-bold text-gray-800">
        Hello World
      </h1>
    </div>
  )
}
```

### CSS Modules

```tsx
// Component.module.css
.container {
  padding: 1rem;
  background: white;
}

// Component.tsx
import styles from './Component.module.css'

export default function Component() {
  return <div className={styles.container}>Content</div>
}
```

## 資料獲取

### fetch

```tsx
async function getData() {
  const res = await fetch('https://api.example.com/data', {
    headers: {
      'Authorization': `Bearer ${process.env.API_TOKEN}`,
    },
    next: { revalidate: 3600 }, // ISR
  })
  
  if (!res.ok) {
    throw new Error('Failed to fetch data')
  }
  
  return res.json()
}
```

### 資料庫查詢

```tsx
import { db } from '@/lib/db'

async function getUsers() {
  const users = await db.query('SELECT * FROM users ORDER BY created_at DESC')
  return users
}
```

### 表單處理

```tsx
// app/actions.ts
'use server'

export async function createUser(formData: FormData) {
  const name = formData.get('name') as string
  const email = formData.get('email') as string
  
  await db.create('users', { name, email })
  
  revalidatePath('/users')
}
```

## 元件

### Image

```tsx
import Image from 'next/image'

export default function Page() {
  return (
    <Image
      src="/photo.jpg"
      alt="Description"
      width={500}
      height={300}
      priority // 預先載入 LCP 圖片
    />
  )
}
```

### Link

```tsx
import Link from 'next/link'

export default function Nav() {
  return (
    <nav>
      <Link href="/">Home</Link>
      <Link href="/about">About</Link>
      <Link href="/blog/nextjs">Blog Post</Link>
    </nav>
  )
}
```

### 字體優化

```tsx
import { Inter } from 'next/font/google'

const inter = Inter({ subsets: ['latin'] })

export default function RootLayout({
  children,
}: {
  children: React.ReactNode
}) {
  return (
    <html lang="zh-TW">
      <body className={inter.className}>{children}</body>
    </html>
  )
}
```

## Middleware

```tsx
// middleware.ts
import { NextResponse } from 'next/server'
import type { NextRequest } from 'next/server'

export function middleware(request: NextRequest) {
  // 驗證 token
  const token = request.cookies.get('token')
  
  if (!token && request.nextUrl.pathname.startsWith('/dashboard')) {
    return NextResponse.redirect(new URL('/login', request.url))
  }
  
  return NextResponse.next()
}

export const config = {
  matcher: ['/dashboard/:path*', '/profile/:path*'],
}
```

## 部署

### Vercel (推薦)

```bash
# 推送至 GitHub 即可自動部署
git push
```

### 自架部署

```bash
# 建置
npm run build

# 啟動
npm start
```

## 環境變數

```bash
# .env.local
DATABASE_URL=postgresql://user:pass@localhost:5432/mydb
API_SECRET=my-secret-key

# .env.production
# 在 Vercel 或其他平臺設定
```

## 最佳實踐

| 實踐 | 說明 |
|------|------|
| 伺服器元件優先 | 減少客戶端 JavaScript |
| 串流 | 使用 Suspense 改善載入體驗 |
| 快取策略 | 善用 revalidate |
| 圖片優化 | 使用 next/image |

## 相關資源

- 相關工具：[React](React.md)
- 相關概念：[Fetch_API](Fetch_API.md)
- 相關主題：[前端框架](前端框架.md)

## Tags

#Next.js #React #SSR #SSG #全端開發

## 範例代碼

- [01-basics.html](../_code/網頁設計/Next.js/01-basics.html) - Next.js 基本範例 (Routing, SSR, SSG, ISR, API Routes, Components)
