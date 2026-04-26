---
tags: [SEO, 搜尋引擎優化, 網站優化, 前端開發]
date: 2026-04-15
source_count: 2
last_updated: 2026-04-15
---

# SEO：搜尋引擎優化

## 概述

搜尋引擎優化（Search Engine Optimization，SEO）是一系列提升網站在搜尋引擎結果頁面（SERP）中排名的技術和策略。良好的 SEO 能為網站帶來免費的有機流量，提升品牌知名度和轉化率。SEO 涵蓋多個層面：技術 SEO（網站結構、爬蟲可訪問性）、頁面 SEO（內容優化、關鍵字策略）、頁外 SEO（外部連結、社群信號）以及使用者體驗信號。

搜尋引擎的工作原理是透過爬蟲（Crawler）發現和抓取網頁，分析頁面內容，建立索引，並根據數百個排名因素決定頁面在特定搜尋查詢中的排名。理解這些原理是有效 SEO 的基礎。現代 SEO 強調提供優質內容、建立良好使用者體驗，並遵循搜尋引擎的指南。

## 技術 SEO

### 網站結構與爬蟲優化

良好的網站結構能幫助搜尋引擎更有效地抓取和理解網站內容。清晰的層級結構、合理的內部連結、以及正確的 robots.txt 和 sitemap 配置都是技術 SEO 的基礎。

```html
<!-- robots.txt 配置 -->
User-agent: *
Allow: /
Disallow: /admin/
Disallow: /private/
Disallow: /api/
Disallow: /checkout/
Disallow: /cart/

Allow: /assets/
Allow: /images/

Sitemap: https://example.com/sitemap.xml
Crawl-delay: 1

# 禁止爬蟲訪問特定爬蟲
User-agent: BadBot
Disallow: /

User-agent: Googlebot
Allow: /
Disallow: /search/
```

```xml
<!-- sitemap.xml -->
<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9"
        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xsi:schemaLocation="http://www.sitemaps.org/schemas/sitemap/0.9
                            http://www.sitemaps.org/schemas/sitemap/0.9/sitemap.xsd">

  <url>
    <loc>https://example.com/</loc>
    <lastmod>2026-04-15</lastmod>
    <changefreq>daily</changefreq>
    <priority>1.0</priority>
  </url>

  <url>
    <loc>https://example.com/products/</loc>
    <lastmod>2026-04-15</lastmod>
    <changefreq>weekly</changefreq>
    <priority>0.9</priority>
  </url>

  <url>
    <loc>https://example.com/products/electronics/</loc>
    <lastmod>2026-04-14</lastmod>
    <changefreq>weekly</changefreq>
    <priority>0.8</priority>
  </url>

  <!-- 動態頁面 -->
  <url>
    <loc>https://example.com/product/123</loc>
    <lastmod>2026-04-15</lastmod>
    <changefreq>monthly</changefreq>
    <priority>0.7</priority>
    <xhtml:link rel="alternate" hreflang="en" href="https://example.com/en/product/123"/>
    <xhtml:link rel="alternate" hreflang="zh-TW" href="https://example.com/zh-TW/product/123"/>
  </url>

</urlset>
```

### Meta 標籤優化

```html
<head>
  <!-- 基本 Meta 標籤 -->
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">

  <!-- 標題標籤 -->
  <title>產品名稱 | 品牌名稱 - 最佳選擇</title>

  <!-- Meta 描述 -->
  <meta name="description" content="這是關於產品的描述，包含主要關鍵字，應該在150-160個字符之間，清楚地說明頁面內容和價值主張。">

  <!-- 關鍵字（大多數搜尋引擎已忽略，但仍有少數使用） -->
  <meta name="keywords" content="關鍵字1, 關鍵字2, 關鍵字3">

  <!-- 作者和版權 -->
  <meta name="author" content="公司名稱">
  <meta name="copyright" content="© 2026 公司名稱">

  <!-- 機器人指示 -->
  <meta name="robots" content="index, follow, max-image-preview:large, max-snippet:-1, max-video-preview:-1">
  <!-- 或更具體的控制 -->
  <meta name="robots" content="noindex, nofollow">

  <!-- Open Graph (Facebook, LinkedIn) -->
  <meta property="og:type" content="product">
  <meta property="og:title" content="產品名稱 | 品牌名稱">
  <meta property="og:description" content="這是產品的描述，在社群媒體分享時顯示。">
  <meta property="og:image" content="https://example.com/images/product-og.jpg">
  <meta property="og:image:width" content="1200">
  <meta property="og:image:height" content="630">
  <meta property="og:url" content="https://example.com/product/123">
  <meta property="og:site_name" content="品牌名稱">
  <meta property="og:locale" content="zh_TW">
  <meta property="product:price:amount" content="999">
  <meta property="product:price:currency" content="TWD">

  <!-- Twitter Cards -->
  <meta name="twitter:card" content="summary_large_image">
  <meta name="twitter:title" content="產品名稱 | 品牌名稱">
  <meta name="twitter:description" content="這是產品的描述，在 Twitter 分享時顯示。">
  <meta name="twitter:image" content="https://example.com/images/product-twitter.jpg">
  <meta name="twitter:site" content="@brandname">
  <meta name="twitter:creator" content="@author">

  <!-- 規範連結（Canonical URL）-->
  <link rel="canonical" href="https://example.com/product/123">

  <!-- 語言聲明 -->
  <link rel="alternate" hreflang="en" href="https://example.com/en/product/123">
  <link rel="alternate" hreflang="zh-TW" href="https://example.com/zh-TW/product/123">
  <link rel="alternate" hreflang="x-default" href="https://example.com/product/123">

  <!-- 圖標 -->
  <link rel="icon" type="image/x-icon" href="/favicon.ico">
  <link rel="icon" type="image/png" sizes="32x32" href="/favicon-32x32.png">
  <link rel="apple-touch-icon" sizes="180x180" href="/apple-touch-icon.png">

  <!-- 結構化數據提示 -->
  <link rel="manifest" href="/manifest.json">

  <!-- 預連接和預獲取 -->
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link rel="dns-prefetch" href="https://www.google-analytics.com">
</head>
```

### 結構化數據（Schema Markup）

結構化數據幫助搜尋引擎更好地理解頁面內容，並可能觸發豐富結果（Rich Snippets）。

```html
<!-- 產品結構化數據 -->
<script type="application/ld+json">
{
  "@context": "https://schema.org",
  "@type": "Product",
  "name": "產品名稱",
  "image": [
    "https://example.com/images/product-1.jpg",
    "https://example.com/images/product-2.jpg"
  ],
  "description": "這是產品的詳細描述，包含重要的關鍵字。",
  "sku": "PROD-12345",
  "gtin13": "0123456789012",
  "brand": {
    "@type": "Brand",
    "name": "品牌名稱"
  },
  "manufacturer": {
    "@type": "Organization",
    "name": "製造商名稱"
  },
  "offers": {
    "@type": "Offer",
    "url": "https://example.com/product/123",
    "priceCurrency": "TWD",
    "price": "999",
    "priceValidUntil": "2026-12-31",
    "availability": "https://schema.org/InStock",
    "itemCondition": "https://schema.org/NewCondition",
    "seller": {
      "@type": "Organization",
      "name": "賣家名稱"
    },
    "shippingDetails": {
      "@type": "OfferShippingDetails",
      "shippingRate": {
        "@type": "MonetaryAmount",
        "value": "100",
        "currency": "TWD"
      },
      "shippingDestination": {
        "@type": "DefinedRegion",
        "addressCountry": "TW"
      },
      "deliveryTime": {
        "@type": "ShippingDeliveryTime",
        "handlingTime": {
          "@type": "QuantitativeValue",
          "minValue": "1",
          "maxValue": "2",
          "unitCode": "DAY"
        },
        "transitTime": {
          "@type": "QuantitativeValue",
          "minValue": "1",
          "maxValue": "3",
          "unitCode": "DAY"
        }
      }
    }
  },
  "aggregateRating": {
    "@type": "AggregateRating",
    "ratingValue": "4.5",
    "reviewCount": "128",
    "bestRating": "5",
    "worstRating": "1"
  },
  "review": [
    {
      "@type": "Review",
      "reviewRating": {
        "@type": "Rating",
        "ratingValue": "5",
        "bestRating": "5"
      },
      "author": {
        "@type": "Person",
        "name": "張小明"
      },
      "datePublished": "2026-03-15",
      "reviewBody": "這是一個非常棒的产品，我非常滿意。"
    }
  ],
  "sku": "PROD-12345"
}
</script>

<!-- 組織結構化數據 -->
<script type="application/ld+json">
{
  "@context": "https://schema.org",
  "@type": "Organization",
  "name": "公司名稱",
  "url": "https://example.com",
  "logo": "https://example.com/logo.png",
  "sameAs": [
    "https://www.facebook.com/company",
    "https://www.instagram.com/company",
    "https://www.linkedin.com/company/company",
    "https://www.youtube.com/channel/company"
  ],
  "contactPoint": {
    "@type": "ContactPoint",
    "telephone": "+886-2-1234-5678",
    "contactType": "customer service",
    "availableLanguage": ["Chinese", "English"],
    "areaServed": "TW"
  },
  "address": {
    "@type": "PostalAddress",
    "streetAddress": "某某路123號",
    "addressLocality": "台北市",
    "addressRegion": "台北市",
    "postalCode": "100",
    "addressCountry": "TW"
  }
}
</script>

<!-- 文章結構化數據 -->
<script type="application/ld+json">
{
  "@context": "https://schema.org",
  "@type": "Article",
  "headline": "文章標題",
  "description": "文章摘要描述",
  "image": "https://example.com/images/article.jpg",
  "author": {
    "@type": "Person",
    "name": "作者名稱",
    "url": "https://example.com/author/author-name"
  },
  "publisher": {
    "@type": "Organization",
    "name": "網站名稱",
    "logo": {
      "@type": "ImageObject",
      "url": "https://example.com/logo.png"
    }
  },
  "datePublished": "2026-04-01",
  "dateModified": "2026-04-15",
  "mainEntityOfPage": {
    "@type": "WebPage",
    "@id": "https://example.com/article/slug"
  },
  "articleSection": "分類",
  "keywords": ["關鍵字1", "關鍵字2", "關鍵字3"],
  "wordCount": 1500
}
</script>

<!-- FAQ 結構化數據 -->
<script type="application/ld+json">
{
  "@context": "https://schema.org",
  "@type": "FAQPage",
  "mainEntity": [
    {
      "@type": "Question",
      "name": "問題1？",
      "acceptedAnswer": {
        "@type": "Answer",
        "text": "這是問題1的答案，包含詳細的解答內容。"
      }
    },
    {
      "@type": "Question",
      "name": "問題2？",
      "acceptedAnswer": {
        "@type": "Answer",
        "text": "這是問題2的答案，包含詳細的解答內容。"
      }
    }
  ]
}
</script>
```

## 頁面 SEO

### 標題和內容優化

```html
<!-- HTML 標題層級結構示例 -->
<article>
  <header>
    <h1>主標題：完整的指南 - 如何選擇最佳產品</h1>
    <p class="intro">這是文章摘要，包含主要關鍵字，幫助讀者和搜尋引擎快速了解內容。</p>
  </header>

  <nav>
    <h2>目錄</h2>
    <ul>
      <li><a href="#section-1">第一部分標題</a></li>
      <li><a href="#section-2">第二部分標題</a></li>
      <li><a href="#section-3">第三部分標題</a></li>
    </ul>
  </nav>

  <main>
    <section id="section-1">
      <h2>H2：第一部分標題 - 包含關鍵字</h2>
      <p>段落內容，介紹第一部分的內容...</p>
      
      <h3>H3：子標題 - 更具體的話題</h3>
      <p>詳細內容...</p>
      
      <h3>H3：另一個子標題</h3>
      <p>更多內容...</p>
    </section>

    <section id="section-2">
      <h2>H2：第二部分標題 - 另一個重要話題</h2>
      <p>內容...</p>
    </section>

    <section id="section-3">
      <h2>H2：第三部分標題</h2>
      <p>內容...</p>
    </section>
  </main>

  <aside>
    <h2>相關文章</h2>
    <ul>
      <li><a href="/related-article-1">相關文章1</a></li>
      <li><a href="/related-article-2">相關文章2</a></li>
    </ul>
  </aside>
</article>
```

### 圖片優化

```html
<!-- 優化圖片的最佳實踐 -->
<figure>
  <img 
    src="/images/product-medium.jpg"
    srcset="
      /images/product-small.jpg 480w,
      /images/product-medium.jpg 800w,
      /images/product-large.jpg 1200w,
      /images/product-xl.jpg 1920w
    "
    sizes="
      (max-width: 600px) 480px,
      (max-width: 1200px) 800px,
      1200px
    "
    alt="產品的描述性替代文字，包含主要關鍵字"
    width="800"
    height="600"
    loading="lazy"
    decoding="async"
    title="滑鼠懸停時顯示的標題"
  >
  <figcaption>圖片說明，描述圖片內容</figcaption>
</figure>

<!-- WebP 格式示例 -->
<picture>
  <source 
    type="image/webp"
    srcset="
      /images/product.webp 1x,
      /images/product@2x.webp 2x
    "
  >
  <source 
    type="image/jpeg"
    srcset="
      /images/product.jpg 1x,
      /images/product@2x.jpg 2x
    "
  >
  <img 
    src="/images/product.jpg" 
    alt="產品描述"
    loading="lazy"
  >
</picture>
```

### 連結優化

```html
<!-- 內部連結 -->
<nav>
  <ul>
    <li><a href="/">首頁</a></li>
    <li><a href="/products/" title="瀏覽所有產品">產品</a></li>
    <li><a href="/about/" title="關於我們">關於</a></li>
    <li><a href="/contact/" title="聯繫我們">聯繫</a></li>
  </ul>
</nav>

<!-- 文章內的內部連結 -->
<p>
  了解更多關於我们的
  <a href="/category/subcategory/" title="查看子分類">子分類</a>。
</p>

<!-- 外部連結（指向權威網站） -->
<p>
  根據 <a href="https://www.wikipedia.org/" target="_blank" rel="noopener noreferrer">維基百科</a> 的定義...
</p>

<!-- nofollow 連結示例 -->
<a href="https://example.com/sponsored" rel="sponsored">讚助內容</a>
<a href="https://example.com/user-generated" rel="ugc">用戶生成內容</a>
<a href="https://example.com/untrusted" rel="nofollow">不可信連結</a>
```

## 效能優化

### Core Web Vitals

Core Web Vitals 是 Google 定義的影響使用者體驗的關鍵指標，直接影響搜尋排名。

```javascript
// 使用 web-vitals 庫收集 Core Web Vitals
import { onCLS, onFID, onLCP, onFCP, onTTFB } from 'web-vitals';

function sendToAnalytics({ name, delta, id, rating }) {
  console.log(`${name}: ${delta}`);
  // 发送到分析服务
  gtag('event', name, {
    event_category: 'Web Vitals',
    event_label: id,
    value: Math.round(name === 'CLS' ? delta * 1000 : delta),
    non_interaction: true,
  });
}

onCLS(sendToAnalytics);  // Cumulative Layout Shift
onFID(sendToAnalytics);  // First Input Delay (改為 INP)
onLCP(sendToAnalytics);  // Largest Contentful Paint
onFCP(sendToAnalytics);  // First Contentful Paint
onTTFB(sendToAnalytics); // Time to First Byte
```

```html
<!-- 預加載關鍵資源 -->
<link rel="preload" href="/fonts/main-font.woff2" as="font" type="font/woff2" crossorigin>
<link rel="preload" href="/css/critical.css" as="style">
<link rel="preload" href="/images/hero.webp" as="image">

<!-- 預取 -->
<link rel="prefetch" href="/next-page.html">
<link rel="dns-prefetch" href="https://analytics.example.com">

<!-- 關鍵 CSS -->
<style>
  /* 渲染關鍵路徑所需的 CSS */
  body { margin: 0; font-family: system-ui, sans-serif; }
  .header { padding: 1rem; background: #333; color: white; }
  .hero { min-height: 50vh; }
</style>
```

### 現代圖片格式和響應式圖片

```html
<!-- 響應式圖片配置 -->
<img 
  src="fallback.jpg"
  srcset="
    small.jpg 480w,
    medium.jpg 800w,
    large.jpg 1200w,
    xlarge.jpg 1920w
  "
  sizes="
    (max-width: 480px) 100vw,
    (max-width: 768px) 480px,
    (max-width: 1200px) 800px,
    1200px
  "
  loading="lazy"
  decoding="async"
  fetchpriority="high"
  alt="描述性替代文字"
>

<!-- 現代圖片格式 -->
<picture>
  <!-- AVIF（最佳壓縮） -->
  <source type="image/avif" srcset="image.avif 1x, image-2x.avif 2x">
  <!-- WebP（良好支援） -->
  <source type="image/webp" srcset="image.webp 1x, image-2x.webp 2x">
  <!-- 原生 JPEG -->
  <img src="image.jpg" srcset="image.jpg 1x, image-2x.jpg 2x" loading="lazy" alt="...">
</picture>

<!-- 背景圖片 -->
<div style="
  background-image: 
    -webkit-image-set(
      url('small.jpg') 1x,
      url('medium.jpg') 2x
    ),
    url('fallback.jpg');
  background-image: 
    image-set(
      url('small.jpg') 1x,
      url('medium.jpg') 2x
    ),
    url('fallback.jpg');
">
  Content
</div>
```

## 國際化 SEO

### 多語言網站配置

```html
<!-- hreflang 標籤（HTML head） -->
<link rel="alternate" hreflang="en" href="https://example.com/en/page/">
<link rel="alternate" hreflang="zh-TW" href="https://example.com/zh-TW/page/">
<link rel="alternate" hreflang="zh-CN" href="https://example.com/zh-CN/page/">
<link rel="alternate" hreflang="ja" href="https://example.com/ja/page/">
<link rel="alternate" hreflang="x-default" href="https://example.com/en/page/">

<!-- 為每個語言版本添加語言切換器 -->
<nav aria-label="Language selector">
  <ul class="language-switcher">
    <li><a href="/en/" hreflang="en" lang="en">English</a></li>
    <li><a href="/zh-TW/" hreflang="zh-TW" lang="zh-TW">繁體中文</a></li>
    <li><a href="/zh-CN/" hreflang="zh-CN" lang="zh-CN">簡體中文</a></li>
    <li><a href="/ja/" hreflang="ja" lang="ja">日本語</a></li>
  </ul>
</nav>
```

```xml
<!-- XML sitemap 中的 hreflang -->
<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9"
        xmlns:xhtml="http://www.w3.org/1999/xhtml">
  
  <url>
    <loc>https://example.com/en/product/123</loc>
    <xhtml:link rel="alternate" hreflang="en" href="https://example.com/en/product/123"/>
    <xhtml:link rel="alternate" hreflang="zh-TW" href="https://example.com/zh-TW/product/123"/>
    <xhtml:link rel="alternate" hreflang="zh-CN" href="https://example.com/zh-CN/product/123"/>
    <xhtml:link rel="alternate" hreflang="x-default" href="https://example.com/en/product/123"/>
    <lastmod>2026-04-15</lastmod>
    <changefreq>weekly</changefreq>
    <priority>0.9</priority>
  </url>

</urlset>
```

## 效能監控與分析

### SEO 監控工具配置

```javascript
// 完整的 SEO 監控腳本
(function() {
  // 確保頁面完全加載後再收集指標
  window.addEventListener('load', function() {
    setTimeout(function() {
      // 收集 Web Vitals
      if ('web-vitals' in window) {
        import('https://unpkg.com/web-vitals@3/dist/web-vitals.iife.js')
          .then(function(webVitals) {
            webVitals.onCLS(sendToAnalytics);
            webVitals.onFID(sendToAnalytics);
            webVitals.onLCP(sendToAnalytics);
          });
      }
    }, 3000);
  });

  // 追蹤內部連結點擊
  document.addEventListener('click', function(e) {
    var link = e.target.closest('a[href^="/"], a[href^="' + location.origin + '"]');
    if (link) {
      // 發送到分析
      console.log('Internal link clicked:', link.href);
    }
  });

  // 追蹤外部連結點擊
  document.addEventListener('click', function(e) {
    var link = e.target.closest('a[href^="http"]');
    if (link && !link.href.startsWith(location.origin)) {
      // 發送到分析
      console.log('External link clicked:', link.href);
      // 如果是附屬連結
      if (link.rel && link.rel.indexOf('sponsored') !== -1) {
        console.log('Sponsored link clicked');
      }
    }
  });

  function sendToAnalytics(metric) {
    // 發送數據到分析服務
    if (typeof gtag !== 'undefined') {
      gtag('event', metric.name, {
        event_category: 'Web Vitals',
        event_label: metric.id,
        value: Math.round(metric.value),
        non_interaction: true
      });
    }
  }
})();
```

## 最佳實踐總結

### SEO 檢查清單

技術 SEO 方面，應確保網站可被抓取（檢查 robots.txt 和 sitemap）、頁面加載速度快（Core Web Vitals達標）、移動設備友好（響應式設計）、使用 HTTPS、URL 結構清晰有意義，以及正確配置 hreflang 標籤。

內容優化方面，標題標籤應獨特且包含關鍵字、Meta 描述有吸引力並包含關鍵字、H1 標籤清晰描述頁面主題、內容高質量且相關、使用正確的標題層級（H1-H6）、圖片有描述性 alt 文字、內部連結策略明確。

結構化數據方面，應添加相關的 Schema.org 標記、使用 JSON-LD 格式、定期驗證結構化數據。

監控優化方面，應使用 Google Search Console 監控索引和排名、使用 Google Analytics 追蹤流量和行為、定期檢查並修復爬蟲錯誤、追蹤 Core Web Vitals 指標。

## 相關頁面

- [單元測試](./單元測試.md) - 自動化測試實踐
- [CI/CD](./CI_CD.md) - 持續整合與部署
- [Webpack 與 Vite](./Webpack與Vite.md) - 前端建構工具

## 標籤

#SEO #搜尋引擎優化 #網站優化 #前端開發 #Core Web Vitals
