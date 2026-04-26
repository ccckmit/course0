# DevOps

DevOps 是「開發」（Development）與「維運」（Operations）的結合，是一種強調開發團隊與維運團隊之間協作的文化、方法論和工具集合。DevOps 的核心目標是縮短軟體交付週期、提高品質、實現持續交付（Continuous Delivery），讓組織能夠更快地回應市場變化和客戶需求。

## DevOps 的起源與發展

### 傳統模式的問題

在傳統的軟體開發模式中，開發團隊和維運團隊通常是分離的：

- **開發團隊**負責編寫程式碼，專注於功能實現和快速迭代
- **維運團隊**負責部署和維護系統，專注於穩定性和可靠性

這種分離導致諸多問題：
- **資訊不對稱**：開發人員不瞭解生產環境，維運人員不瞭解程式碼
- **衝突目標**：開發追求快速交付，維運追求穩定運行
- **部署瓶頸**：部署過程繁瑣、容易出錯，導致延遲上線
- **責任推諉**：問題發生時，團隊互相指責而非協作解決

### DevOps 的誕生

2009 年左右，Flickr 工程師在一場名爲「10+ Deploys Per Day」的演講中，首次提出了 DevOps 的概念。此後，DevOps 逐漸成為軟體工程領域的主流實踐。

## DevOps 核心原則

### 1. 文化（Culture）

DevOps 首先是一種文化轉變，強調：

- **協作文化**：開發與維運團隊共同承擔責任
- **共享知識**：透明分享資訊，消除知識壁壘
- **持續改進**：不斷優化流程和工具
- **承擔風險**：勇於嘗試新技術，快速學習失敗

### 2. 自動化（Automation）

自動化是 DevOps 的核心實踐：

- **建構自動化**：自動化編譯和打包
- **測試自動化**：自動化執行各類測試
- **部署自動化**：自動化部署到各環境
- **基礎設施自動化**：基礎設施即程式碼（IaC）

### 3. 持續改進（Continuous Improvement）

- **監控回饋**：即時監控系統狀態，快速響應問題
- **度量分析**：收集和分析關鍵指標，持續優化
- **快速迭代**：小步快跑，快速驗證假設

### 4. 客戶導向（Customer Focus）

- **快速交付**：更快地將價值傳遞給客戶
- **收集回饋**：根據客戶回饋調整方向
- **持續優化**：根據使用數據優化產品

## DevOps 生命週期

```
┌─────────────────────────────────────────────────────────────────┐
│                        DevOps 生命週期                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐    │
│   │  規劃   │───→│  開發   │───→│  測試   │───→│  部署   │    │
│   └─────────┘    └─────────┘    └─────────┘    └─────────┘    │
│        │              │              │              │           │
│        └──────────────┴──────────────┴──────────────┘         │
│                              │                                  │
│                              ▼                                  │
│                        ┌─────────┐                             │
│                        │  監控   │                             │
│                        └─────────┘                             │
│                              │                                  │
│                              └────────────────────────────────  │
│                                                                 │
│                        ┌─────────┐                             │
│                        │ 反饋優化 │                             │
│                        └─────────┘                             │
└─────────────────────────────────────────────────────────────────┘
```

### 1. 規劃（Plan）

- 敏捷方法論：Scrum、Kanban
- 需求管理：User Story、Backlog
- 專案追蹤：Jira、Trello、Azure DevOps

### 2. 開發（Develop）

- **版本控制**：Git、GitHub、GitLab
- **程式碼審查**：Pull Request、Code Review
- **程式碼品質**：SonarQube、ESLint
- **知識共享**：Wiki、技術分享

### 3. 測試（Test）

- **單元測試**：JUnit、pytest
- **整合測試**：Postman、SoapUI
- **端到端測試**：Selenium、Cypress
- **效能測試**：JMeter、Locust

### 4. 部署（Deploy）

- **容器化**：Docker
- **容器編排**：Kubernetes
- **自動化部署**：CI/CD
- **基礎設施**：Terraform、CloudFormation

### 5. 監控（Monitor）

- **日誌收集**：ELK、Loki
- **指標監控**：Prometheus、Grafana
- **追蹤**：Jaeger、Zipkin
- **告警**：PagerDuty、Opsgenie

### 6. 反饋優化（Feedback）

- **回饋收集**：使用者回饋、數據分析
- **持續改進**：根據反饋優化流程
- **快速修復**：快速響應問題

## CI/CD（持續整合/持續交付）

CI/CD 是 DevOps 的核心實踐，實現軟體的自動化和持續交付。

### 持續整合（Continuous Integration, CI）

每次程式碼變更自動觸發建構和測試：

```
┌─────────────────────────────────────────────────────────────────┐
│                        CI 流程                                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   開發者 push   ┌────────┐   自動化建構   ┌────────┐            │
│   ────────────→│  Git   │──────────────→│ CI伺服器│            │
│                 └────────┘               └────┬───┘            │
│                                               │                 │
│                                               ▼                 │
│                                       ┌──────────────┐          │
│                                       │  自動化測試   │          │
│                                       └──────┬───────┘          │
│                                              │                  │
│                              ┌───────────────┴───────────────┐ │
│                              ▼                               │ │
│                       ┌──────────┐         ┌──────────┐     │ │
│                       │ 建構成功  │         │ 建構失敗  │     │ │
│                       └──────────┘         └──────────┘     │ │
│                              │                               │ │
│                              └───────────────┬───────────────┘ │
│                                          回饋                  │
└─────────────────────────────────────────────────────────────────┘
```

**CI 的核心實踐**：

1. **頻繁提交**：開發者每天多次提交程式碼
2. **自動化建構**：每次提交自動觸發建構
3. **自動化測試**：每次建構執行完整測試套件
4. **快速反饋**：建構失敗時立即通知團隊
5. **保持建構健康**：修復失敗的建構是最高優先級

### 持續交付（Continuous Delivery, CD）

自動化將程式碼部署到測試/預備環境：

```
┌─────────────────────────────────────────────────────────────────┐
│                        CD 流程                                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   CI 完成  →  自動化測試  →  部署到測試環境  →  部署到預備環境   │
│                                                                 │
│   ┌─────────┐   ┌─────────┐   ┌─────────────┐   ┌───────────┐   │
│   │ Build   │──→│  Test   │──→│ Staging     │──→│ Production│   │
│   └─────────┘   └─────────┘   └─────────────┘   └───────────┘   │
│                                                                 │
│                 自動化                          自動化（可選）   │
└─────────────────────────────────────────────────────────────────┘
```

**持續交付的核心實踐**：

1. **自動化部署**：一鍵部署到任何環境
2. **部署管線**：定義清晰的部署階段
3. **功能開關**：控制功能上線
4. **藍綠部署**：零停機部署
5. **金絲雀發布**：逐步放量

### 持續部署（Continuous Deployment）

完全自動化，程式碼通過所有測試後自動部署到生產環境。

### CI/CD 工具比較

| 工具 | 類型 | 特點 |
|------|------|------|
| Jenkins | 開源 | 功能強大、靈活性高、插件豐富 |
| GitHub Actions | 雲端 | 與 GitHub 深度整合 |
| GitLab CI | 雲端/自託管 | 一體化 DevOps 平臺 |
| Azure Pipelines | 雲端 | 支援多種語言和平臺 |
| CircleCI | 雲端 | 快速、易用 |
| Argo CD | 雲端 | GitOps 風格的 CD |

### CI/CD 設定範例

```yaml
# GitHub Actions 範例
name: CI/CD Pipeline

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Set up Node.js
        uses: actions/setup-node@v3
        with:
          node-version: '18'
          
      - name: Install Dependencies
        run: npm ci
        
      - name: Run Linter
        run: npm run lint
        
      - name: Run Tests
        run: npm test
        
      - name: Build
        run: npm run build
        
      - name: Upload Build Artifact
        uses: actions/upload-artifact@v3
        with:
          name: build
          path: dist/

  deploy:
    needs: build
    runs-on: ubuntu-latest
    if: github.ref == 'refs/heads/main'
    
    steps:
      - uses: actions/download-artifact@v3
        with:
          name: build
          
      - name: Deploy to Production
        run: |
          echo "Deploying to production..."
```

## Infrastructure as Code（基礎設施即程式碼）

IaC 是用程式碼管理和配置基礎設施的實踐，實現基礎設施的版本化、可重現和自動化。

### 常用 IaC 工具

| 工具 | 類別 | 特點 |
|------|------|------|
| Terraform | 宣告式 | 支援多雲、狀態管理 |
| CloudFormation | 雲端原生 | AWS 專屬 |
| Pulumi | 程式設計式 | 使用熟悉語言 |
| Ansible | 配置管理 | Agentless、SSH |
| Chef | 配置管理 | 客戶端-伺服器 |
| Puppet | 配置管理 | 宣告式 |

### Terraform 範例

```hcl
# main.tf
terraform {
  required_version = ">= 1.0"
  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 4.0"
    }
  }
}

provider "aws" {
  region = "us-east-1"
}

# 建立 VPC
resource "aws_vpc" "main" {
  cidr_block           = "10.0.0.0/16"
  enable_dns_hostnames = true
  enable_dns_support   = true
  
  tags = {
    Name = "main-vpc"
  }
}

# 建立子網路
resource "aws_subnet" "public" {
  vpc_id                  = aws_vpc.main.id
  cidr_block             = "10.0.1.0/24"
  availability_zone       = "us-east-1a"
  map_public_ip_on_launch = true
  
  tags = {
    Name = "public-subnet"
  }
}

# 建立 EC2 實例
resource "aws_instance" "web" {
  ami           = "ami-0c55b159cbfafe1f0"
  instance_type = "t2.micro"
  subnet_id     = aws_subnet.public.id
  
  tags = {
    Name = "web-server"
  }
}

# 建立安全羣組
resource "aws_security_group" "web" {
  name        = "web-sg"
  description = "Security group for web server"
  vpc_id      = aws_vpc.main.id
  
  ingress {
    from_port   = 80
    to_port     = 80
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
  
  ingress {
    from_port   = 443
    to_port     = 443
    protocol    = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
  
  egress {
    from_port   = 0
    to_port     = 0
    protocol    = "-1"
    cidr_blocks = ["0.0.0.0/0"]
  }
}
```

### Ansible 範例

```yaml
# playbook.yml
---
- name: Web Server Setup
  hosts: webservers
  become: yes
  
  tasks:
    - name: Update apt cache
      apt:
        update_cache: yes
      when: ansible_os_family == "Debian"
    
    - name: Install Nginx
      apt:
        name: nginx
        state: present
    
    - name: Start Nginx service
      service:
        name: nginx
        state: started
        enabled: yes
    
    - name: Copy Nginx configuration
      template:
        src: nginx.conf.j2
        dest: /etc/nginx/nginx.conf
      notify: Restart Nginx
    
    - name: Deploy application
      git:
        repo: https://github.com/example/app.git
        dest: /var/www/app
        version: main
    
  handlers:
    - name: Restart Nginx
      service:
        name: nginx
        state: restarted
```

## 監控與日誌

### 監控堆疊

```
┌─────────────────────────────────────────────────────────────────┐
│                        監控架構                                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌────────────┐   ┌────────────┐   ┌────────────┐             │
│   │  指標收集   │   │   日誌收集  │   │   追蹤收集 │             │
│   └─────┬──────┘   └─────┬──────┘   └─────┬──────┘             │
│         │                │                │                    │
│         ▼                ▼                ▼                    │
│   ┌────────────┐   ┌────────────┐   ┌────────────┐             │
│   │ Prometheus │   │    ELK     │   │   Jaeger   │             │
│   └─────┬──────┘   └─────┬──────┘   └─────┬──────┘             │
│         │                │                │                    │
│         └────────────────┴────────────────┘                    │
│                          │                                     │
│                          ▼                                     │
│                  ┌──────────────┐                             │
│                  │   Grafana    │                             │
│                  │   視覺化儀表  │                             │
│                  └──────────────┘                             │
└─────────────────────────────────────────────────────────────────┘
```

### 日誌管理：ELK Stack

- **Elasticsearch**：分散式搜尋引擎，儲存和檢索日誌
- **Logstash**：日誌收集、處理和轉換
- **Kibana**：日誌視覺化和分析
- **Filebeat**：輕量級日誌收集器

```yaml
# docker-compose.yml for ELK Stack
version: '3'
services:
  elasticsearch:
    image: docker.elastic.co/elasticsearch/elasticsearch:7.17.0
    environment:
      - discovery.type=single-node
      - "ES_JAVA_OPTS=-Xms512m -Xmx512m"
    ports:
      - "9200:9200"
    volumes:
      - elasticsearch-data:/usr/share/elasticsearch/data
  
  kibana:
    image: docker.elastic.co/kibana/kibana:7.17.0
    ports:
      - "5601:5601"
    depends_on:
      - elasticsearch
  
  logstash:
    image: docker.elastic.co/logstash/logstash:7.17.0
    volumes:
      - ./logstash.conf:/usr/share/logstash/pipeline/logstash.conf
    ports:
      - "5044:5044"
    depends_on:
      - elasticsearch

volumes:
  elasticsearch-data:
```

### 監控指標

- **基礎設施指標**：CPU、記憶體、磁碟、網路
- **應用指標**：請求延遲、錯誤率、吞吐量
- **業務指標**：DAU、轉化率、營收
- **安全指標**：登入失敗、異常存取

```yaml
# Prometheus 監控配置
global:
  scrape_interval: 15s

scrape_configs:
  - job_name: 'node'
    static_configs:
      - targets: ['localhost:9100']
  
  - job_name: 'prometheus'
    static_configs:
      - targets: ['localhost:9090']
  
  - job_name: 'application'
    metrics_path: '/metrics'
    static_configs:
      - targets: ['localhost:8080']
```

## 常見 DevOps 工具生態

### 版本控制與協作

- **Git**：版本控制系統
- **GitHub/GitLab**：Git 託管平臺
- **Jira**：專案管理
- **Confluence**：知識庫

### CI/CD

- **Jenkins**：自動化伺服器
- **GitHub Actions**：自動化工作流
- **GitLab CI**：持續整合/交付
- **Argo CD**：Kubernetes 原生 CD

### 容器與編排

- **Docker**：容器平臺
- **Kubernetes**：容器編排
- **Helm**：Kubernetes 包管理器
- **Istio**：服務網格

### 監控與日誌

- **Prometheus**：指標監控
- **Grafana**：視覺化
- **ELK Stack**：日誌管理
- **Jaeger**：分散式追蹤

### 雲端平臺

- **AWS**：Amazon Web Services
- **GCP**：Google Cloud Platform
- **Azure**：Microsoft Azure

## DevOps 成熟度模型

### Level 1: 初始級

- 手工部署
- 無標準化流程
- 缺乏監控

### Level 2: 可管理

- 有版本控制
- 有基本 CI
- 有簡單監控

### Level 3: 已定義

- 完整 CI/CD
- Infrastructure as Code
- 自動化測試

### Level 4: 量化管理

- 持續監控
- 度量驅動優化
- 自動化修復

### Level 5: 持續優化

- 自驅動優化
- 預測分析
- 持續創新

## 相關主題

- [持續整合](持續整合.md) - CI 詳細說明
- [持續部署](持續部署.md) - CD 詳細說明
- [Docker](Docker.md) - 容器化技術
- [Kubernetes](../系統程式/Kubernetes.md) - 容器編排
- [監控與日誌](監控與日誌.md) - 監控實踐
- [版本控制](版本控制.md) - Git 使用