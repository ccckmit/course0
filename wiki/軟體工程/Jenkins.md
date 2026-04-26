# Jenkins

## 概述

Jenkins 是最流行的開源自動化伺服器，支援持續整合和持續交付（CI/CD），可用於自動化建置、測試和部署軟體。

## 安裝和基本設定

### Docker 安裝
```bash
docker run -d \
  --name jenkins \
  -p 8080:8080 \
  -p 50000:50000 \
  -v jenkins_home:/var/jenkins_home \
  jenkins/jenkins:lts
```

### 主要設定
- **外掛管理**：安裝 Git、Docker、Kubernetes 等外掛
- **Credentials**：管理 SSH、金鑰、API Token
- **工具設定**：設定 JDK、Node.js、Python 等路徑

## Pipeline 語法

### Declarative Pipeline
```groovy
pipeline {
    agent any
    
    environment {
        APP_NAME = 'my-app'
        DOCKER_REGISTRY = 'registry.example.com'
    }
    
    options {
        buildDiscarder(logRotator(numToKeepStr: '10'))
        timeout(time: 30, unit: 'MINUTES')
    }
    
    stages {
        stage('Checkout') {
            steps {
                git branch: 'main',
                    url: 'https://github.com/org/repo.git'
            }
        }
        
        stage('Build') {
            steps {
                sh '''
                    npm ci
                    npm run build
                '''
            }
        }
        
        stage('Test') {
            steps {
                sh 'npm run test:ci'
                junit 'reports/*.xml'
            }
        }
        
        stage('Security Scan') {
            steps {
                sh 'trivy image ${DOCKER_REGISTRY}/${APP_NAME}:${BRANCH_NAME}'
            }
        }
        
        stage('Deploy to Staging') {
            when {
                branch 'main'
            }
            steps {
                sh '''
                    kubectl set image deployment/${APP_NAME} \\
                        ${APP_NAME}=${DOCKER_REGISTRY}/${APP_NAME}:${GIT_COMMIT}
                '''
            }
        }
        
        stage('Deploy to Production') {
            when {
                branch 'main'
            }
            steps {
                input message: 'Deploy to Production?',
                      ok: 'Deploy'
                sh '''
                    kubectl set image deployment/${APP_NAME} \\
                        ${APP_NAME}=${DOCKER_REGISTRY}/${APP_NAME}:${GIT_COMMIT}
                '''
            }
        }
    }
    
    post {
        always {
            cleanWs()
        }
        success {
            slackSend channel: '#deployments',
                      message: "Build ${BUILD_NUMBER} succeeded!"
        }
        failure {
            slackSend channel: '#deployments',
                      message: "Build ${BUILD_NUMBER} failed!"
        }
    }
}
```

### Scripted Pipeline
```groovy
node('docker') {
    stage('Checkout') {
        checkout scm
    }
    
    stage('Build Image') {
        def image = docker.build("${env.REGISTRY}/${env.APP_NAME}")
    }
    
    stage('Push Image') {
        docker.withRegistry("https://${env.REGISTRY}", 'docker-registry') {
            image.push("${env.BRANCH_NAME}-${env.GIT_COMMIT}")
            image.push('latest')
        }
    }
}
```

## 平行化和矩陣化

### Parallel Stages
```groovy
pipeline {
    stages {
        stage('Test') {
            parallel {
                stage('Unit Tests') {
                    steps {
                        sh 'npm run test:unit'
                    }
                }
                stage('Integration Tests') {
                    steps {
                        sh 'npm run test:integration'
                    }
                }
                stage('E2E Tests') {
                    steps {
                        sh 'npm run test:e2e'
                    }
                }
            }
        }
    }
}
```

### Matrix Configuration
```groovy
pipeline {
    stages {
        stage('Build Matrix') {
            matrix {
                axes {
                    axis {
                        name 'PLATFORM'
                        values 'linux', 'windows', 'macos'
                    }
                    axis {
                        name 'ARCH'
                        values 'amd64', 'arm64'
                    }
                }
                stages {
                    stage('Build') {
                        steps {
                            sh "make build PLATFORM=${PLATFORM} ARCH=${ARCH}"
                        }
                    }
                }
            }
        }
    }
}
```

## 共享函式庫 (Shared Libraries)

### 定義共享庫
```groovy
// vars/buildImage.groovy
def call(Map config) {
    pipeline {
        docker.build("${config.registry}/${config.image}")
    }
}

// vars/slackNotify.groovy
def call(String status) {
    slackSend color: status == 'SUCCESS' ? 'good' : 'danger',
              message: "${status}: Job '${env.JOB_NAME}' [${env.BUILD_NUMBER}]"
}
```

### 使用共享庫
```groovy
@Library('my-shared-library') _

pipeline {
    stages {
        stage('Build') {
            steps {
                buildImage registry: 'registry.example.com',
                           image: 'my-app'
            }
        }
        stage('Notify') {
            steps {
                slackNotify 'SUCCESS'
            }
        }
    }
}
```

## Credentials 管理

| 類型 | 使用場景 |
|------|----------|
| Username/Password | Docker Hub, GitHub |
| Secret Text | API Keys, Tokens |
| SSH Username with Private Key | Git SSH, Server Login |
| Certificate | TLS/SSL 憑證 |

## Jenkinsfile 範例（多環境部署）
```groovy
pipeline {
    environment {
        DOCKER_REGISTRY = 'registry.example.com'
    }
    
    stages {
        stage('Prepare') {
            steps {
                script {
                    env.IMAGE_TAG = "${env.BRANCH_NAME}-${env.GIT_COMMIT[0..7]}"
                }
            }
        }
        
        stage('Build & Test') {
            stages {
                stage('Build') {
                    steps {
                        sh 'docker build -t ${IMAGE_TAG} .'
                    }
                }
                stage('Unit Tests') {
                    steps {
                        sh 'docker run ${IMAGE_TAG} npm test'
                    }
                }
            }
        }
        
        stage('Security Scan') {
            steps {
                sh "trivy image --exit-code 1 --severity HIGH,CRITICAL ${IMAGE_TAG}"
            }
        }
        
        stage('Push to Registry') {
            when {
                anyOf { branch 'main'; branch 'develop' }
            }
            steps {
                withCredentials([usernamePassword(
                    credentialsId: 'docker-registry',
                    usernameVariable: 'DOCKER_USER',
                    passwordVariable: 'DOCKER_PASS'
                )]) {
                    sh '''
                        docker login -u $DOCKER_USER -p $DOCKER_PASS ${DOCKER_REGISTRY}
                        docker tag ${IMAGE_TAG} ${DOCKER_REGISTRY}/${IMAGE_TAG}
                        docker push ${DOCKER_REGISTRY}/${IMAGE_TAG}
                    '''
                }
            }
        }
        
        stage('Deploy') {
            matrix {
                axes {
                    axis {
                        name 'ENVIRONMENT'
                        values 'staging', 'production'
                    }
                }
                stages {
                    stage("Deploy to ${ENVIRONMENT}") {
                        when {
                            expression { ENVIRONMENT == 'staging' || branch == 'main' }
                        }
                        steps {
                            sh """
                                kubectl config use-context ${ENVIRONMENT}
                                kubectl set image deployment/app app=${DOCKER_REGISTRY}/${IMAGE_TAG}
                            """
                        }
                    }
                }
            }
        }
    }
}
```

## 觸發條件

```groovy
triggers {
    // 定時觸發
    cron('H 2 * * *')
    
    // GitHub webhook
    githubPush()
    
    // 輪詢 SCM
    pollSCM('H/5 * * * *')
}
```

## 最佳實踐

1. **使用 Jenkinsfile**：將 Pipeline 程式碼化
2. **失敗快速回饋**：第一階段失敗就停止
3. **隔離環境**：使用 Docker 隔離建置環境
4. **Artifact 保存**：保留建置產物供後續使用
5. **安全設定**：啟用存取控制，限制 Credentials 暴露

## 相關資源

- 相關主題：[持續整合](持續整合.md) - CI/CD 流程
- 相關主題：[持續部署](持續部署.md) - CD 實踐
- 相關工具：Git（見 [持續整合](持續整合.md)）

## Tags

#Jenkins #CI/CD #自動化建置
