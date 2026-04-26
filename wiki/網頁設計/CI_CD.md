---
tags: [DevOps, CI/CD, GitHub Actions, 自動化部署, 持續整合]
date: 2026-04-15
source_count: 2
last_updated: 2026-04-15
---

# CI/CD：持續整合與持續部署

## 概述

持續整合（Continuous Integration，CI）和持續部署（Continuous Deployment，CD）是現代軟體開發的核心實踐，統稱為 CI/CD。持續整合要求開發者頻繁地將程式碼變更合併到共享主線，每次提交都會觸發自動化構建和測試，確保程式碼品質。持續部署則將通過所有測試的變更自動部署到生產環境，實現快速可靠的軟體發布。

CI/CD 的核心價值在於：**快速反饋**（及早發現問題）、**降低風險**（小步驟部署易於回滾）、**提高效率**（自動化取代手動操作）、**提升品質**（標準化的測試和部署流程）、**加速交付**（頻繁發布新功能）。這套實踐起源於極限編程（XP），現已成為現代 DevOps 的基石。

## GitHub Actions 詳解

### 基礎概念

GitHub Actions 是 GitHub 提供的 CI/CD 平台，允許在 GitHub 倉庫中直接定義自動化工作流程。工作流程由一個或多個作業（Job）組成，每個作業包含多個步驟（Step），每個步驟執行一個操作（Action）或命令。工作流程觸發器（Trigger）定義了何時執行工作流程，可以是推送、PR、定時任務或手動觸發。

```yaml
# .github/workflows/main.yml
name: CI/CD Pipeline

on:
  push:
    branches: [main, develop]
    tags:
      - 'v*'
  pull_request:
    branches: [main]
  workflow_dispatch:
    inputs:
      environment:
        description: 'Deployment environment'
        required: true
        default: 'staging'

env:
  NODE_VERSION: '18.x'
  REGISTRY: ghcr.io
  IMAGE_NAME: ${{ github.repository }}

jobs:
  lint:
    name: Lint Code
    runs-on: ubuntu-latest
    steps:
      - name: Checkout code
        uses: actions/checkout@v4
        
      - name: Setup Node.js
        uses: actions/setup-node@v4
        with:
          node-version: ${{ env.NODE_VERSION }}
          cache: 'npm'
          
      - name: Install dependencies
        run: npm ci
        
      - name: Run ESLint
        run: npm run lint
        
      - name: Check types
        run: npm run typecheck

  test:
    name: Unit Tests
    runs-on: ubuntu-latest
    needs: lint
    services:
      postgres:
        image: postgres:15
        env:
          POSTGRES_USER: test
          POSTGRES_PASSWORD: test
          POSTGRES_DB: test
        ports:
          - 5432:5432
        options: >-
          --health-cmd pg_isready
          --health-interval 10s
          --health-timeout 5s
          --health-retries 5
          
      redis:
        image: redis:7-alpine
        ports:
          - 6379:6379
        options: >-
          --health-cmd "redis-cli ping"
          --health-interval 10s
          --health-timeout 5s
          --health-retries 5
    
    steps:
      - uses: actions/checkout@v4
      
      - name: Setup Node.js
        uses: actions/setup-node@v4
        with:
          node-version: ${{ env.NODE_VERSION }}
          cache: 'npm'
          
      - name: Install dependencies
        run: npm ci
        
      - name: Run tests
        run: npm run test:coverage
        env:
          DATABASE_URL: postgresql://test:test@localhost:5432/test
          REDIS_URL: redis://localhost:6379
          
      - name: Upload coverage
        uses: codecov/codecov-action@v3
        with:
          token: ${{ secrets.CODECOV_TOKEN }}
          files: ./coverage/lcov.info
          flags: unittests
          name: codecov-umbrella
          
      - name: Upload coverage reports
        uses: actions/upload-artifact@v4
        with:
          name: coverage-report
          path: coverage/
          retention-days: 7

  build:
    name: Build Docker Image
    runs-on: ubuntu-latest
    needs: test
    outputs:
      image-tag: ${{ steps.meta.outputs.tags }}
      image-digest: ${{ steps.build.outputs.digest }}
    
    steps:
      - name: Checkout code
        uses: actions/checkout@v4
        
      - name: Set up Docker Buildx
        uses: docker/setup-buildx-action@v3
        
      - name: Log in to Container Registry
        uses: docker/login-action@v3
        with:
          registry: ${{ env.REGISTRY }}
          username: ${{ github.actor }}
          password: ${{ secrets.GITHUB_TOKEN }}
          
      - name: Extract metadata
        id: meta
        uses: docker/metadata-action@v5
        with:
          images: ${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}
          tags: |
            type=ref,event=branch
            type=ref,event=pr
            type=semver,pattern={{version}}
            type=sha,prefix=
            
      - name: Build and push
        uses: docker/build-push-action@v5
        with:
          context: .
          push: ${{ github.event_name != 'pull_request' }}
          tags: ${{ steps.meta.outputs.tags }}
          labels: ${{ steps.meta.outputs.labels }}
          cache-from: type=gha
          cache-to: type=gha,mode=max
          outputs: type=image,name=${{ env.REGISTRY }}/${{ env.IMAGE_NAME }},push-by-digest=true
          store: true
          
      - name: Export digest
        id: build
        run: |
          mkdir -p /tmp/digests
          digest="${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}@${{ steps.buildx.outputs.digest }}"
          touch "/tmp/digests/${digest#*:}"
          
  deploy-staging:
    name: Deploy to Staging
    runs-on: ubuntu-latest
    needs: build
    if: github.event_name == 'push' && github.ref == 'refs/heads/develop'
    environment:
      name: staging
      url: https://staging.example.com
    
    steps:
      - name: Deploy to staging cluster
        uses: appleboy/ssh-action@v1
        with:
          host: ${{ secrets.STAGING_HOST }}
          username: ${{ secrets.STAGING_USER }}
          key: ${{ secrets.STAGING_SSH_KEY }}
          script: |
            kubectl set image deployment/app \
              app=${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}@${{ needs.build.outputs.image-digest }}
            kubectl rollout status deployment/app --timeout=300s
            
  deploy-production:
    name: Deploy to Production
    runs-on: ubuntu-latest
    needs: build
    if: startsWith(github.ref, 'refs/tags/v')
    environment:
      name: production
      url: https://example.com
    concurrency:
      group: production
      cancel-in-progress: false
    
    steps:
      - name: Deploy to production
        uses: appleboy/ssh-action@v1
        with:
          host: ${{ secrets.PRODUCTION_HOST }}
          username: ${{ secrets.PRODUCTION_USER }}
          key: ${{ secrets.PRODUCTION_SSH_KEY }}
          script: |
            kubectl set image deployment/app \
              app=${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}@${{ needs.build.outputs.image-digest }}
            kubectl rollout status deployment/app --timeout=600s
            kubectl exec -it deployment/app -- /scripts/health-check.sh
```

### 矩陣策略與條件執行

```yaml
# .github/workflows/matrix.yml
name: Multi-Platform Build

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  build:
    name: Build on ${{ matrix.os }}
    runs-on: ${{ matrix.os }}
    strategy:
      fail-fast: false
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        node-version: [16.x, 18.x, 20.x]
        exclude:
          - os: windows-latest
            node-version: 20.x
          - os: macos-latest
            node-version: 16.x
    
    steps:
      - uses: actions/checkout@v4
      
      - name: Setup Node.js ${{ matrix.node-version }}
        uses: actions/setup-node@v4
        with:
          node-version: ${{ matrix.node-version }}
          
      - name: Install dependencies
        run: npm ci
        
      - name: Build
        run: npm run build
        shell: bash
        
      - name: Test
        run: npm test
        
      - name: Upload artifacts
        uses: actions/upload-artifact@v4
        with:
          name: build-${{ matrix.os }}-node-${{ matrix.node-version }}
          path: dist/

  e2e-test:
    name: E2E Tests
    runs-on: ubuntu-latest
    strategy:
      matrix:
        shard: [1, 2, 3, 4]
    steps:
      - uses: actions/checkout@v4
      
      - name: Setup Node.js
        uses: actions/setup-node@v4
        with:
          node-version: 18.x
          
      - name: Install dependencies
        run: npm ci
        
      - name: Install Playwright
        run: npx playwright install --with-deps
        
      - name: Run E2E tests
        run: npx playwright test --shard=${{ matrix.shard }}/${{ strategy.job.total }}
        
      - name: Upload test results
        if: always()
        uses: actions/upload-artifact@v4
        with:
          name: playwright-report-shard-${{ matrix.shard }}
          path: playwright-report/
```

### 快取策略

```yaml
# .github/workflows/caching.yml
name: Build with Caching

on:
  push:
    branches: [main]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
      - uses: actions/checkout@v4
      
      # npm 快取
      - name: Cache npm dependencies
        uses: actions/cache@v4
        id: npm-cache
        with:
          path: ~/.npm
          key: ${{ runner.os }}-npm-${{ hashFiles('**/package-lock.json') }}
          restore-keys: |
            ${{ runner.os }}-npm-
            
      # node_modules 快取
      - name: Cache node_modules
        uses: actions/cache@v4
        with:
          path: node_modules
          key: ${{ runner.os }}-modules-${{ hashFiles('**/package-lock.json') }}
          restore-keys: |
            ${{ runner.os }}-modules-
            
      # pip/Python 快取
      - name: Cache pip packages
        uses: actions/cache@v4
        with:
          path: ~/.cache/pip
          key: ${{ runner.os }}-pip-${{ hashFiles('**/requirements.txt') }}
          restore-keys: |
            ${{ runner.os }}-pip-
            
      # Docker 層快取
      - name: Build image
        uses: docker/build-push-action@v5
        with:
          cache-from: type=gha
          cache-to: type=gha,mode=max
            
      # Rust cargo 快取
      - name: Cache Rust dependencies
        uses: Swatinem/rust-cache@v2
        with:
          workspaces: './src -> target'
          
      - name: Install dependencies
        run: npm ci
```

### 部署策略與回滾

```yaml
# .github/workflows/deploy.yml
name: Deployment Pipeline

on:
  push:
    branches: [main]
    tags:
      - 'v*'

env:
  KUBECONFIG: /tmp/kubeconfig

jobs:
  deploy:
    name: Deploy Application
    runs-on: ubuntu-latest
    environment: ${{ github.ref == 'refs/heads/main' && 'staging' || 'preview' }}
    
    steps:
      - name: Checkout
        uses: actions/checkout@v4
        
      - name: Setup kubectl
        run: |
          echo "${{ secrets.KUBE_CONFIG }}" | base64 -d > $KUBECONFIG
          
      - name: Deploy application
        run: |
          # 備份當前版本
          kubectl get deployment app -o yaml > /tmp/backup.yaml
          
          # 部署新版本
          kubectl set image deployment/app \
            app=${{ env.IMAGE_NAME }}:${{ github.sha }}
            
          # 滾動更新監控
          kubectl rollout status deployment/app --timeout=300s
          
      - name: Run smoke tests
        run: |
          sleep 10
          curl -f https://${{ secrets.DOMAIN }}/health || exit 1
          
      - name: Notify success
        if: success()
        uses: slackapi/slack-github-action@v1
        with:
          payload: |
            {
              "text": "Deployment successful!",
              "blocks": [{
                "type": "section",
                "text": {
                  "type": "mrkdwn",
                  "text": "*Deployment Successful*\nEnvironment: production\nCommit: ${{ github.sha }}"
                }
              }]
            }
        env:
          SLACK_WEBHOOK_URL: ${{ secrets.SLACK_WEBHOOK_URL }}
          SLACK_WEBHOOK_TYPE: INCOMING_WEBHOOK

  rollback:
    name: Rollback
    runs-on: ubuntu-latest
    if: failure() && github.ref == 'refs/heads/main'
    environment:
      name: production
    needs: deploy
    
    steps:
      - name: Checkout
        uses: actions/checkout@v4
        
      - name: Setup kubectl
        run: |
          echo "${{ secrets.KUBE_CONFIG }}" | base64 -d > $KUBECONFIG
          
      - name: Rollback deployment
        run: |
          kubectl rollout undo deployment/app
          kubectl rollout status deployment/app --timeout=300s
          
      - name: Notify failure
        uses: slackapi/slack-github-action@v1
        with:
          payload: |
            {
              "text": "Deployment failed and rolled back!",
              "blocks": [{
                "type": "section",
                "text": {
                  "type": "mrkdwn",
                  "text": "*Deployment Failed - Rolling Back*\nEnvironment: production\nCommit: ${{ github.sha }}"
                }
              }]
            }
        env:
          SLACK_WEBHOOK_URL: ${{ secrets.SLACK_WEBHOOK_URL }}
```

## 自動化測試策略

### 多層測試流程

```yaml
# .github/workflows/test-pipeline.yml
name: Test Pipeline

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  # 1. 快速lint階段
  lint:
    name: Lint
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
        with:
          node-version: 18.x
          cache: 'npm'
      - run: npm ci
      - run: npm run lint
      - run: npm run typecheck

  # 2. 單元測試階段
  unit-tests:
    name: Unit Tests
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
        with:
          node-version: 18.x
          cache: 'npm'
      - run: npm ci
      - run: npm run test:unit
      - uses: actions/upload-artifact@v4
        if: always()
        with:
          name: unit-test-results
          path: coverage/

  # 3. 集成測試階段
  integration-tests:
    name: Integration Tests
    runs-on: ubuntu-latest
    services:
      postgres:
        image: postgres:15
        env:
          POSTGRES_USER: test
          POSTGRES_PASSWORD: test
          POSTGRES_DB: test
        ports:
          - 5432:5432
        options: >-
          --health-cmd pg_isready
          --health-interval 10s
          --health-timeout 5s
          --health-retries 5
          
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
        with:
          node-version: 18.x
          cache: 'npm'
      - run: npm ci
      - run: npm run test:integration
        env:
          DATABASE_URL: postgresql://test:test@localhost:5432/test
      - uses: actions/upload-artifact@v4
        if: always()
        with:
          name: integration-test-results
          path: test-results/

  # 4. E2E測試階段
  e2e-tests:
    name: E2E Tests
    runs-on: ubuntu-latest
    if: github.event_name == 'push' && github.ref == 'refs/heads/main'
    
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
        with:
          node-version: 18.x
          cache: 'npm'
      - run: npm ci
      - run: npm run build
      - name: Start application
        run: npm run start &
      - name: Wait for app
        run: npx wait-on http://localhost:3000
      - name: Run Playwright
        run: npx playwright test
      - uses: actions/upload-artifact@v4
        if: always()
        with:
          name: playwright-report
          path: playwright-report/

  # 5. 安全掃描階段
  security-scan:
    name: Security Scan
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Run npm audit
        run: npm audit --audit-level=high
      - name: Run Snyk security scan
        uses: snyk/actions/node@master
        env:
          SNYK_TOKEN: ${{ secrets.SNYK_TOKEN }}
      - name: Run Trivy vulnerability scanner
        uses: aquasecurity/trivy-action@master
        with:
          scan-type: 'fs'
          scan-ref: '.'
          format: 'sarif'
          output: 'trivy-results.sarif'
      - name: Upload Trivy results to GitHub Security
        uses: github/codeql-action/upload-sarif@v2
        with:
          sarif_file: 'trivy-results.sarif'

  # 6. 性能測試階段
  performance-tests:
    name: Performance Tests
    runs-on: ubuntu-latest
    if: github.event_name == 'push' && github.ref == 'refs/heads/main'
    
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
        with:
          node-version: 18.x
          cache: 'npm'
      - run: npm ci
      - run: npm run build
      - name: Run Lighthouse CI
        uses: treosh/lighthouse-ci-action@v10
        with:
          urls: https://example.com
          budgetPath: ./lighthouse-budget.json
          uploadArtifacts: true
```

## Kubernetes 部署

### Helm Chart 部署

```yaml
# deployment.yaml - GitHub Actions 中的 Kubernetes 部署
name: Deploy to Kubernetes

on:
  push:
    branches: [main]

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        
      - name: Authenticate to Kubernetes
        uses: azure/k8s-set-context@v3
        with:
          kubeconfig: ${{ secrets.KUBE_CONFIG }}
          
      - name: Setup Helm
        uses: azure/setup-helm@v3
        with:
          version: '3.12.0'
          
      - name: Install application
        run: |
          helm upgrade --install app ./charts/app \
            --namespace production \
            --create-namespace \
            --values ./charts/app/values.yaml \
            --set image.repository=${{ env.REGISTRY }}/${{ env.IMAGE_NAME }} \
            --set image.tag=${{ github.sha }} \
            --wait \
            --timeout 10m \
            --atomic
```

### 藍綠部署和金絲雀發布

```yaml
# blue-green-deployment.yml
jobs:
  deploy-canary:
    name: Canary Deployment
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Deploy canary version
        run: |
          kubectl argo rollouts set image rollout/app \
            app=${{ env.IMAGE_NAME }}:${{ github.sha }}
            
      - name: Wait for canary
        run: |
          kubectl argo rollouts status rollout/app \
            --watch --timeout=600s
            
      - name: Promote canary
        if: success()
        run: |
          kubectl argo rollouts promote rollout/app
          
      - name: Abort if unhealthy
        if: failure()
        run: |
          kubectl argo rollouts abort rollout/app
          echo "Canary deployment failed, rolling back..."
```

## 監控與通知

### 監控配置

```yaml
# .github/workflows/monitoring.yml
name: Monitoring

on:
  push:
    branches: [main]

jobs:
  health-check:
    name: Health Check
    runs-on: ubuntu-latest
    steps:
      - name: Checkout
        uses: actions/checkout@v4
        
      - name: Run health checks
        run: |
          for i in {1..10}; do
            response=$(curl -s -o /dev/null -w "%{http_code}" https://example.com/health)
            if [ "$response" -eq 200 ]; then
              echo "Health check passed"
              exit 0
            fi
            echo "Attempt $i failed, retrying..."
            sleep 5
          done
          echo "Health check failed after 10 attempts"
          exit 1

  notify:
    name: Notify Deployment
    runs-on: ubuntu-latest
    needs: [deploy, health-check]
    if: always()
    
    steps:
      - name: Notify on Slack
        if: always()
        uses: slackapi/slack-github-action@v1
        with:
          payload: |
            {
              "text": "Deployment ${{ github.event_name == 'push' && 'succeeded' || 'failed' }}",
              "blocks": [
                {
                  "type": "section",
                  "text": {
                    "type": "mrkdwn",
                    "text": "*Deployment Status:* ${{ needs.deploy.result == 'success' && '✅ Succeeded' || '❌ Failed' }}\n*Environment:* production\n*Branch:* ${{ github.ref_name }}\n*Commit:* ${{ github.sha }}"
                  }
                }
              ]
            }
        env:
          SLACK_WEBHOOK_URL: ${{ secrets.SLACK_WEBHOOK_URL }}
```

## 最佳實踐

### 安全最佳實踐

```yaml
# 安全注意事項
jobs:
  secure-build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          persist-credentials: false
          fetch-depth: 0
          
      - name: Use secret scanning
        uses: actions/secret-scanning@v1
        
      - name: Set up CodeQL
        uses: github/codeql-action/init@v2
        with:
          languages: javascript, typescript
          
      - name: Run CodeQL analysis
        uses: github/codeql-action/analyze@v2
```

### 工作流優化

```yaml
# 優化 CI/CD 工作流
name: Optimized CI

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  # 並行執行獨立的檢查
  lint-and-typecheck:
    name: Lint & Type Check
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
        with:
          node-version: 18.x
          cache: 'npm'
      - run: npm ci
      - run: npm run lint && npm run typecheck

  test:
    name: Tests
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
        with:
          node-version: 18.x
          cache: 'npm'
      - run: npm ci
      - run: npm test

  # 使用複合操作減少步驟
  build-and-push:
    name: Build & Push
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: docker/setup-buildx-action@v3
      - uses: docker/login-action@v3
        with:
          registry: ghcr.io
          username: ${{ github.actor }}
          password: ${{ secrets.GITHUB_TOKEN }}
      - uses: docker/build-push-action@v5
        with:
          push: true
          tags: ghcr.io/${{ github.repository }}:${{ github.sha }}
```

## 總結

CI/CD 是現代軟體開發不可或缺的實踐，GitHub Actions 提供了強大且靈活的自動化平台。有效的 CI/CD 流程應包含：全面的自動化測試（從單元測試到 E2E 測試）、安全的部署流程（帶有回滾機制）、高效的緩存策略、以及完善的監控和通知系統。持續優化工作流程、縮短反饋週期、確保部署可靠性，是建立優秀 CI/CD 系統的關鍵。

## 相關頁面

- [單元測試](./單元測試.md) - 自動化測試實踐
- [Webpack 與 Vite](./Webpack與Vite.md) - 前端建構工具
- [SEO](./SEO.md) - 搜尋引擎優化

## 標籤

#DevOps #CI/CD #GitHub Actions #自動化部署 #持續整合
