import random

cn_keywords = [
    "電腦", "軟體", "硬體", "網路", "網際網路", "資料", "資料庫",
    "安全", "演算法", "程式設計", "語言", "系統", "平台",
    "雲端", "伺服器", "用戶端", "應用程式", "開發", "設計", "測試",
    "除錯", "效能", "優化", "儲存", "記憶體", "處理器", "顯示卡",
    "機器", "學習", "深度", "神經", "人工", "智慧", "人工智慧",
    "網頁", "前端", "後端", "全端", "介面", "框架", "庫",
    "自動化", "監控", "日誌", "錯誤", "警告", "資訊", "偵錯",
    "容器", "叢集", "節點", "執行", "部署", "版本", "分支"
]

en_keywords = [
    "computer", "software", "hardware", "network", "internet", "data", "database",
    "security", "algorithm", "programming", "language", "system", "platform",
    "cloud", "server", "client", "application", "development", "design", "testing",
    "debugging", "performance", "optimization", "storage", "memory", "cpu", "gpu",
    "machine", "learning", "deep", "neural", "artificial", "intelligence",
    "web", "frontend", "backend", "fullstack", "interface", "framework", "library",
    "automation", "monitoring", "logging", "error", "warning", "info", "debug",
    "container", "cluster", "node", "execute", "deploy", "version", "branch"
]

cn_templates = [
    "{kw1}的{kw2}需要{kw3}來處理。",
    "{kw1}與{kw2}在{kw3}平台上運作。",
    "當{kw1}時，考慮使用{kw2}和{kw3}。",
    "一個{kw1}可以透過{kw2}來{kw3}。",
    "{kw1}必須與{kw2}配合才能{kw3}。",
    "{kw1}{kw2}適用於{kw3}環境。",
    "使用{kw1}來{kw2}{kw3}的方法。",
    "這個{kw1}是由{kw2}在{kw3}中實現的。",
    "{kw1}和{kw2}共同來{kw3}{kw4}。",
    "此{kw1}支援跨{kw2}的{kw3}功能。"
]

en_templates = [
    "The {adj} {noun} requires {noun2} to {verb} the {noun3}.",
    "{noun} {verb} {adj} {noun2} through {noun3} and {noun4}.",
    "When {verb}ing the {noun}, consider the {adj} {noun2}.",
    "A {adj} {noun} can {verb} {noun2} using {noun3}.",
    "The {noun} must {verb} with the {adj} {noun2} to {verb2} {noun3}.",
    "{noun} {verb} {adj} {noun2} for {noun3} in {noun4}.",
    "To {verb} {noun}, use a {adj} {noun2} with {noun3}.",
    "The {noun} is {verb}ed by the {adj} {noun2} in {noun3}.",
    "{adj} {noun} and {adj2} {noun2} work together to {verb} {noun3}.",
    "This {noun} {verb}s {noun2} across {adj} {noun3} platforms."
]

adj = ["advanced", "modern", "efficient", "distributed", "scalable", "reliable", "secure", "fast", "optimized", "automated"]
verb = ["process", "manage", "handle", "execute", "optimize", "analyze", "generate", "update", "deploy", "monitor"]
noun = ["system", "application", "service", "platform", "framework", "module", "component", "interface", "architecture", "infrastructure"]
noun2 = ["data", "request", "response", "resource", "endpoint", "configuration", "metadata", "cache", "queue", "stream"]
noun3 = ["server", "client", "network", "database", "storage", "processor", "memory", "bandwidth", "latency", "throughput"]
noun4 = ["container", "cluster", "node", "instance", "pod", "worker", "thread", "process", "session", "connection"]
verb2 = ["improve", "enhance", "enable", "support", "facilitate", "ensure", "provide", "deliver", "achieve", "maintain"]

random.seed(42)
sentences = []

for i in range(500):
    t = random.choice(cn_templates)
    s = t.format(
        kw1=random.choice(cn_keywords),
        kw2=random.choice(cn_keywords),
        kw3=random.choice(cn_keywords),
        kw4=random.choice(cn_keywords)
    )
    sentences.append(f"{i+1}. {s}")

for i in range(500):
    t = random.choice(en_templates)
    s = t.format(
        adj=random.choice(adj),
        adj2=random.choice(adj),
        verb=random.choice(verb),
        verb2=random.choice(verb2),
        noun=random.choice(noun),
        noun2=random.choice(noun2),
        noun3=random.choice(noun3),
        noun4=random.choice(noun4)
    )
    sentences.append(f"{501+i}. {s}")

random.shuffle(sentences)
for i, s in enumerate(sentences):
    sentences[i] = f"{i+1}. {s.split('. ', 1)[1]}"

with open("_data/corpus.txt", "w") as f:
    f.write("\n".join(sentences))

print(f"Generated {len(sentences)} sentences (500 CN + 500 EN)")
