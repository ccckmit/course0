# Kubernetes

Kubernetes（簡稱 K8s）是一個開源的容器編排平臺，用於自動化容器的部署、擴展和管理。Google 最初開發了 Kubernetes 作為內部 Borg 系統的開源版本，於 2015 年發布並捐贈給 CNCF（雲端原生計算基金會）。Kubernetes 已成為雲端原生計算的標準平臺，讓開發者和運維團隊能夠以聲明式的方式管理容器化應用，實現高可用、可擴展、易於維護的系統架構。

## Kubernetes 核心概念

### 為什麼需要 Kubernetes？

```
┌─────────────────────────────────────────────────────────────┐
│              容器管理面臨的挑戰                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   1. 多容器協調                                             │
│      - 容器需要相互通信                                     │
│      - 需要服務發現                                         │
│      - 需要負載平衡                                         │
│                                                             │
│   2. 擴展與收縮                                             │
│      - 手動部署耗時                                         │
│      - 難以應對流量高峯                                     │
│      - 需要自動化                                           │
│                                                             │
│   3. 滾動更新                                               │
│      - 不中斷服務                                           │
│      - 回滾能力                                             │
│      - 版本管理                                             │
│                                                             │
│   4. 自我修復                                               │
│      - 自動重啟失敗的容器                                   │
│      - 替換不健康的節點                                     │
│      - 高可用                                               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Kubernetes 架構

```
┌─────────────────────────────────────────────────────────────┐
│                   Kubernetes 架構                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   ┌─────────────────────────────────────────────────────┐   │
│   │                  Control Plane                      │   │
│   │  ┌─────────┐  ┌─────────┐  ┌─────────┐             │   │
│   │  │ API     │  │ Scheduler│  │ Controller│           │   │
│   │  │ Server  │  │         │  │ Manager   │           │   │
│   │  └─────────┘  └─────────┘  └─────────┘             │   │
│   │  ┌─────────┐  ┌─────────┐                         │   │
│   │  │ etcd    │  │ DNS     │                         │   │
│   │  │         │  │ (CoreDNS)│                         │   │
│   │  └─────────┘  └─────────┘                         │   │
│   └─────────────────────────────────────────────────────┘   │
│                         │                                    │
│                         ▼                                    │
│   ┌─────────────────────────────────────────────────────┐   │
│   │                     Nodes                            │   │
│   │  ┌─────────┐    ┌─────────┐    ┌─────────┐          │   │
│   │  │ Node 1  │    │ Node 2  │    │ Node 3  │          │   │
│   │  │ ┌─────┐ │    │ ┌─────┐ │    │ ┌─────┐ │          │   │
│   │  │ │ kubelet│   │ │ kubelet│   │ │ kubelet│          │   │
│   │  │ └─────┘ │    │ └─────┘ │    │ └─────┘ │          │   │
│   │  │ ┌─────┐ │    │ ┌─────┐ │    │ ┌─────┐ │          │   │
│   │  │ │kube-proxy│  │ │kube-proxy│  │ │kube-proxy│        │   │
│   │  │ └─────┘ │    │ └─────┘ │    │ └─────┘ │          │   │
│   │  └─────────┘    └─────────┘    └─────────┘          │   │
│   └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Control Plane（控制平面）**
- API Server：提供 REST API，處理所有請求
- Scheduler：決定 Pod 應該在哪個節點運行
- Controller Manager：維護期望狀態
- etcd：儲存叢集資料

**Node（工作節點）**
- kubelet：與 API Server 溝通，管理容器
- kube-proxy：網路代理，實現服務發現
- Container Runtime：運行容器（通常是 Docker）

## 核心資源物件

### 1. Pod

Pod 是 Kubernetes 的最小部署單元，一個 Pod 可以包含一個或多個容器。

```yaml
# basic-pod.yaml
apiVersion: v1
kind: Pod
metadata:
  name: my-app
  labels:
    app: my-app
    tier: frontend
spec:
  containers:
  - name: nginx
    image: nginx:1.21
    ports:
    - containerPort: 80
    resources:
      requests:
        memory: "128Mi"
        cpu: "250m"
      limits:
        memory: "256Mi"
        cpu: "500m"
    env:
    - name: ENV_VAR
      value: "production"
    volumeMounts:
    - name: data
      mountPath: /data
  volumes:
  - name: data
    emptyDir: {}
```

### 2. ReplicaSet

確保指定數量的 Pod 副本始終運行：

```yaml
# replicaset.yaml
apiVersion: apps/v1
kind: ReplicaSet
metadata:
  name: my-app-rs
spec:
  replicas: 3
  selector:
    matchLabels:
      app: my-app
  template:
    metadata:
      labels:
        app: my-app
    spec:
      containers:
      - name: my-app
        image: my-app:latest
```

### 3. Deployment

提供宣告式的 Pod 管理，支持滾動更新和回滾：

```yaml
# deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-app
  labels:
    app: my-app
spec:
  replicas: 3
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxSurge: 1
      maxUnavailable: 0
  selector:
    matchLabels:
      app: my-app
  template:
    metadata:
      labels:
        app: my-app
    spec:
      containers:
      - name: my-app
        image: my-app:v1
        ports:
        - containerPort: 8080
        readinessProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 5
          periodSeconds: 10
        livenessProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 15
          periodSeconds: 20
        resources:
          requests:
            memory: "256Mi"
            cpu: "200m"
          limits:
            memory: "512Mi"
            cpu: "500m"
      affinity:
        podAntiAffinity:
          preferredDuringSchedulingIgnoredDuringExecution:
          - weight: 100
            podAffinityTerm:
              labelSelector:
                matchExpressions:
                - key: app
                  operator: In
                  values:
                  - my-app
              topologyKey: kubernetes.io/hostname
```

### 4. Service

為 Pod 集合提供穩定的網路端點：

```yaml
# service.yaml
apiVersion: v1
kind: Service
metadata:
  name: my-app-svc
spec:
  type: ClusterIP  # ClusterIP, NodePort, LoadBalancer, ExternalName
  selector:
    app: my-app
  ports:
  - protocol: TCP
    port: 80
    targetPort: 8080
  sessionAffinity: ClientIP
  sessionAffinityConfig:
    clientIP:
      timeoutSeconds: 10800
```

**Service 類型**：
- **ClusterIP**：僅在叢集內可訪問（預設）
- **NodePort**：在每個節點的特定端口暴露
- **LoadBalancer**：使用雲端負載平衡器
- **ExternalName**：DNS CNAME 別名

```yaml
# NodePort Service
apiVersion: v1
kind: Service
metadata:
  name: my-app-nodeport
spec:
  type: NodePort
  selector:
    app: my-app
  ports:
  - port: 80
    targetPort: 8080
    nodePort: 30080
```

### 5. ConfigMap

儲存非敏感的配置資料：

```yaml
# configmap.yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: my-app-config
data:
  database_url: "postgres://db:5432/app"
  log_level: "info"
  app.conf: |
    [server]
    port = 8080
    timeout = 30
```

### 6. Secret

儲存敏感資料（密碼、Token、密鑰）：

```yaml
# secret.yaml
apiVersion: v1
kind: Secret
metadata:
  name: my-app-secret
type: Opaque
stringData:
  username: admin
  password: changeme
```

### 7. Ingress

HTTP/HTTPS 路由到 Services：

```yaml
# ingress.yaml
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: my-app-ingress
  annotations:
    nginx.ingress.kubernetes.io/rewrite-target: /
spec:
  ingressClassName: nginx
  rules:
  - host: app.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: my-app-svc
            port:
              number: 80
  tls:
  - hosts:
    - app.example.com
    secretName: app-tls
```

### 8. StatefulSet

有狀態應用的部署（支援穩定網路標識、持久存儲）：

```yaml
# statefulset.yaml
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: mysql
spec:
  serviceName: mysql
  replicas: 3
  selector:
    matchLabels:
      app: mysql
  template:
    metadata:
      labels:
        app: mysql
    spec:
      containers:
      - name: mysql
        image: mysql:8.0
        volumeMounts:
        - name: data
          mountPath: /var/lib/mysql
  volumeClaimTemplates:
  - metadata:
      name: data
    spec:
      accessModes: ["ReadWriteOnce"]
      resources:
        requests:
          storage: 10Gi
```

### 9. DaemonSet

在每個節點運行一個 Pod：

```yaml
# daemonset.yaml
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: log-collector
spec:
  selector:
    matchLabels:
      app: log-collector
  template:
    metadata:
      labels:
        app: log-collector
    spec:
      containers:
      - name: fluentd
        image: fluentd:latest
        volumeMounts:
        - name: varlog
          mountPath: /var/log
```

## 資源管理

### 資源配額

```yaml
# resource-quota.yaml
apiVersion: v1
kind: ResourceQuota
metadata:
  name: my-quota
spec:
  hard:
    requests.cpu: "4"
    requests.memory: 8Gi
    limits.cpu: "8"
    limits.memory: 16Gi
    pods: "20"
    services: "10"
```

### LimitRange

```yaml
# limit-range.yaml
apiVersion: v1
kind: LimitRange
metadata:
  name: my-limits
spec:
  limits:
  - default:
      memory: 512Mi
      cpu: 500m
    defaultRequest:
      memory: 256Mi
      cpu: 200m
    max:
      memory: 2Gi
      cpu: "2"
    min:
      memory: 128Mi
      cpu: 100m
    type: Container
```

## 網路與安全

### NetworkPolicy

```yaml
# network-policy.yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: app-policy
spec:
  podSelector:
    matchLabels:
      app: my-app
  policyTypes:
  - Ingress
  - Egress
  ingress:
  - from:
    - podSelector:
        matchLabels:
          role: frontend
    ports:
    - protocol: TCP
      port: 8080
  egress:
  - to:
    - podSelector:
        matchLabels:
          role: database
    ports:
    - protocol: TCP
      port: 5432
```

### Role-Based Access Control (RBAC)

```yaml
# role.yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: Role
metadata:
  name: app-reader
rules:
- apiGroups: [""]
  resources: ["pods", "services"]
  verbs: ["get", "list", "watch"]
---
# rolebinding.yaml
apiVersion: rbac.authorization.k8s.io/v1
kind: RoleBinding
metadata:
  name: read-pods
subjects:
- kind: User
  name: john
  apiGroup: rbac.authorization.k8s.io
roleRef:
  kind: Role
  name: app-reader
  apiGroup: rbac.authorization.k8s.io
```

## 儲存

### PersistentVolume

```yaml
# pv.yaml
apiVersion: v1
kind: PersistentVolume
metadata:
  name: my-pv
spec:
  capacity:
    storage: 10Gi
  accessModes:
  - ReadWriteOnce
  persistentVolumeReclaimPolicy: Retain
  storageClassName: standard
  nfs:
    server: nfs-server.example.com
    path: /exports
```

### PersistentVolumeClaim

```yaml
# pvc.yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: my-pvc
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 5Gi
  storageClassName: standard
```

## 調度與排程

### 節點選擇

```yaml
# node-selector
spec:
  nodeSelector:
    disktype: ssd
```

### 親和性與反親和性

```yaml
spec:
  affinity:
    nodeAffinity:
      preferredDuringSchedulingIgnoredDuringExecution:
      - weight: 100
        preference:
          matchExpressions:
          - key: workload-type
            operator: In
            values:
            - compute-intensive
    podAntiAffinity:
      requiredDuringSchedulingIgnoredDuringExecution:
      - labelSelector:
          matchExpressions:
          - key: app
            operator: In
            values:
            - my-app
        topologyKey: kubernetes.io/hostname
```

### 污點與容忍

```污點節點>
kubectl taint nodes node1 key=value:NoSchedule
kubectl taint nodes node1 key=value:NoExecute
kubectl taint nodes node1 key=value:PreferNoSchedule
```

```容忍（在 Pod 中）>
spec:
  tolerations:
  - key: "key"
    operator: "Equal"
    value: "value"
    effect: "NoSchedule"
```

## 監控與日誌

### 健康檢查

```yaml
spec:
  containers:
  - name: web
    readinessProbe:
      httpGet:
        path: /ready
        port: 8080
      initialDelaySeconds: 5
      periodSeconds: 10
    livenessProbe:
      httpGet:
        path: /health
        port: 8080
      initialDelaySeconds: 15
      periodSeconds: 20
      failureThreshold: 3
    startupProbe:
      httpGet:
        path: /startup
        port: 8080
      failureThreshold: 30
      periodSeconds: 10
```

### 資源監控

```bash
# 查看資源使用
kubectl top nodes
kubectl top pods

# 查看詳細資訊
kubectl describe pod my-pod
kubectl logs my-pod
kubectl logs my-pod --previous  # 之前的容器日誌
```

## 常見命令

```bash
# 部署應用
kubectl apply -f deployment.yaml
kubectl apply -f ./

# 查看資源
kubectl get pods
kubectl get services
kubectl get deployments
kubectl get all

# 描述資源
kubectl describe pod my-pod

# 進入容器
kubectl exec -it my-pod -- /bin/bash

# 擴展
kubectl scale deployment my-app --replicas=5

# 滾動更新
kubectl set image deployment/my-app my-app=my-app:v2

# 回滾
kubectl rollout status deployment/my-app
kubectl rollout undo deployment/my-app

# 刪除
kubectl delete -f deployment.yaml
kubectl delete pods --all

# 調試
kubectl port-forward svc/my-app 8080:80
kubectl debug -it pod/my-app --image=busybox
```

## Helm - Kubernetes 包管理器

```bash
# 安裝 Helm
curl https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3 | bash

# 添加 repo
helm repo add stable https://charts.helm.sh/stable
helm repo update

# 搜尋 chart
helm search repo nginx

# 安裝
helm install my-nginx stable/nginx-ingress

# 升級
helm upgrade my-nginx stable/nginx-ingress

# 回滾
helm rollback my-nginx 1

# 創建 chart
helm create my-chart
```

## Istio - 服務網格

Istio 提供連接、安全、控制和觀測服務的統一方式：

```yaml
# VirtualService
apiVersion: networking.istio.io/v1beta1
kind: VirtualService
metadata:
  name: my-app
spec:
  hosts:
  - my-app
  http:
  - route:
    - destination:
        host: my-app
        subset: v1
      weight: 90
    - destination:
        host: my-app
        subset: v2
      weight: 10
```

## 相關主題

- [Docker](Docker.md) - 容器化技術
- [雲端技術](雲端技術.md) - 雲端運算
- [DevOps](../軟體工程/DevOps.md) - DevOps 實踐
- [容器編排](../軟體工程/容器編排.md) - 容器編排概念