# Docker

## 概述

Docker 是一個開源容器化平臺，將應用程式及其相依環境封裝為可攜式容器，實現「建置一次，到處執行」的目標。

## 核心概念

### 容器 vs 虛擬機
```
┌─────────────────┐     ┌─────────────────┐
│      VM         │     │    Container    │
├─────────────────┤     ├─────────────────┤
│   Guest OS      │     │   App + Deps    │
│   + App         │     │   + Libraries   │
│   + Libraries   │     ├─────────────────┤
│   + Libraries   │     │   Docker Engine │
├─────────────────┤     ├─────────────────┤
│   Hypervisor    │     │   Host Kernel   │
│   + Hardware    │     │   + Hardware    │
└─────────────────┘     └─────────────────┘
```

### 核心元件
- **Image**：唯讀模板，定義容器內容
- **Container**：Image 的執行實例
- **Registry**：存放和分發 Image（Docker Hub、ECR、GCR）
- **Dockerfile**：定義 Image 建置指令

## 基本指令

### Image 操作
```bash
# 拉取 Image
docker pull ubuntu:22.04

# 列出本地 Image
docker images

# 刪除 Image
docker rmi ubuntu:22.04

# 建置 Image
docker build -t my-app:1.0 .

# 登入 Registry
docker login registry.example.com
docker push registry.example.com/my-app:1.0
```

### Container 操作
```bash
# 執行容器
docker run -d --name my-container -p 8080:80 nginx

# 列出執行中的容器
docker ps

# 列出所有容器
docker ps -a

# 停止/啟動容器
docker stop my-container
docker start my-container

# 刪除容器
docker rm my-container

# 查看容器日誌
docker logs -f my-container

# 進入容器
docker exec -it my-container /bin/bash
```

## Dockerfile 最佳實踐

### 多階段建置
```dockerfile
# 建置階段
FROM node:18-alpine AS builder
WORKDIR /app
COPY package*.json ./
RUN npm ci --only=production
COPY . .
RUN npm run build

# 執行階段
FROM node:18-alpine AS runner
WORKDIR /app
ENV NODE_ENV=production

# 複製建置產物
COPY --from=builder /app/dist ./dist
COPY --from=builder /app/node_modules ./node_modules

# 使用非 root 用戶
RUN addgroup -g 1001 appgroup && \
    adduser -u 1001 -G appgroup -D appuser
    
USER appuser

EXPOSE 3000
CMD ["node", "dist/index.js"]
```

### .dockerignore
```dockerignore
node_modules
.git
.gitignore
*.md
.env*
dist
coverage
.vscode
.DS_Store
```

### 層層快取優化
```dockerfile
# 將不常變動的相依先複製
FROM python:3.11-slim

WORKDIR /app

# 先複製相依檔案
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# 再複製應用程式碼
COPY . .

CMD ["python", "app.py"]
```

## Docker Compose

### 服務編排
```yaml
# docker-compose.yml
version: '3.8'

services:
  app:
    build: .
    ports:
      - "3000:3000"
    environment:
      - DATABASE_URL=postgres://db:5432/app
      - REDIS_URL=redis://cache:6379
    depends_on:
      db:
        condition: service_healthy
      cache:
        condition: service_started
    volumes:
      - ./data:/app/data
    networks:
      - backend

  db:
    image: postgres:15-alpine
    environment:
      POSTGRES_DB: app
      POSTGRES_USER: user
      POSTGRES_PASSWORD: password
    volumes:
      - postgres_data:/var/lib/postgresql/data
    healthcheck:
      test: ["CMD-SHELL", "pg_isready -U user -d app"]
      interval: 10s
      timeout: 5s
      retries: 5
    networks:
      - backend

  cache:
    image: redis:7-alpine
    networks:
      - backend

  nginx:
    image: nginx:alpine
    ports:
      - "80:80"
    volumes:
      - ./nginx.conf:/etc/nginx/nginx.conf:ro
    depends_on:
      - app
    networks:
      - backend

volumes:
  postgres_data:

networks:
  backend:
    driver: bridge
```

## 網路模式

| 模式 | 說明 | 使用場景 |
|------|------|----------|
| bridge | 預設網路，容器間可通信 | 單一主機 |
| host | 容器直接使用主機網路 | 效能敏感應用 |
| overlay | 跨主機網路 | Docker Swarm |
| none | 無網路 | 安全隔離 |

```bash
# 建立自訂網路
docker network create my-network

# 執行時指定網路
docker run --network my-network my-app
```

## 資料管理

### Volume 類型
```yaml
services:
  app:
    volumes:
      # 命名 volume
      - app_data:/var/lib/app/data
      
      # Bind mount（主機目錄）
      - ./config:/app/config:ro
      
      # Tmpfs（記憶體）
      - type: tmpfs
        target: /app/sensitive
        tmpfs:
          size: 1000
```

### 資料備份
```bash
# 備份 volume
docker run --rm -v app_data:/data -v $(pwd):/backup alpine \
  tar czf /backup/backup.tar.gz /data

# 還原 volume
docker volume create new_data
docker run --rm -v new_data:/data -v $(pwd):/backup alpine \
  tar xzf /backup/backup.tar.gz -C /
```

## 安全最佳實踐

```dockerfile
# 使用特定版本而非 latest
FROM node:18.17.0-alpine3.18

# 非 root 用戶
RUN addgroup -g 1001 -S nodejs && \
    adduser -S nodeuser -u 1001 -G nodejs
USER nodeuser

# 複製檔案所有權
COPY --chown=nodeuser:nodejs . .

# 設定 read-only root filesystem
docker run --read-only --tmpfs /tmp my-app

# 不使用 privileged 模式
```

### 安全掃描
```bash
# Trivy 漏洞掃描
trivy image my-app:1.0

# Docker Scout
docker scout cves my-app:1.0
```

## 資源限制

```bash
# CPU 和記憶體限制
docker run \
  --memory="256m" \
  --memory-swap="512m" \
  --cpus="1.5" \
  --cpu-shares=1024 \
  my-app
```

## 常見除錯

```bash
# 查看容器程序
docker top my-container

# 檢查容器配置
docker inspect my-container

# 容器效能統計
docker stats

# 確認埠口映射
docker port my-container

# 進入網路命名空間
docker exec -it my-container sh
```

## 相關資源

- 相關主題：[容器編排](容器編排.md) - Kubernetes 管理多容器
- 相關主題：[持續整合](持續整合.md) - CI/CD 中的 Docker 使用
- 相關主題：[雲端原生](雲端原生.md) - 雲端容器化應用

## Tags

#Docker #容器化 #DevOps
