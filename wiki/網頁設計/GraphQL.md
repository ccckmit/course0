# GraphQL

GraphQL 是一種 API 查詢語言，由 Facebook 於 2015 年開源。它允許客戶端精確指定所需的資料，避免了傳統 REST API 的過度獲取或不足獲取問題。

## 核心概念

### 對比 REST

REST API：
```
GET /api/users/1          → 回傳所有用戶欄位
GET /api/users/1/posts     → 回傳所有文章
GET /api/posts/1/comments → 回傳所有留言
```

GraphQL：
```graphql
query {
    user(id: 1) {
        name
        email
        posts {
            title
        }
    }
}
```

### 單一端點

GraphQL 只使用一個端點：

```
POST /graphql
```

所有查詢都透過這個端點，差異在於請求主體。

## Schema 定義

### 基本類型

```graphql
type User {
    id: ID!
    name: String!
    email: String!
    age: Int
    isActive: Boolean!
    createdAt: DateTime!
}

# 標記說明：
# ! = 非空（必填）
# ID = 唯一識別符
# String = UTF-8 字串
# Int = 32 位元整數
# Float = 雙精度浮點數
# Boolean = true/false
```

### 查詢類型（Query）

```graphql
type Query {
    # 基本欄位
    users: [User!]!
    user(id: ID!): User
    
    # 帶參數
    posts(filter: PostFilter, limit: Int = 10): [Post!]!
    post(id: ID!): Post
    
    # 巢狀查詢
    user(id: ID!) {
        name
        friends {
            name
            friends {
                name  # 可達任意深度
            }
        }
    }
}
```

### 變動類型（Mutation）

```graphql
type Mutation {
    createUser(input: CreateUserInput!): User!
    updateUser(id: ID!, input: UpdateUserInput!): User
    deleteUser(id: ID!): Boolean!
}

input CreateUserInput {
    name: String!
    email: String!
    password: String!
}

input UpdateUserInput {
    name: String
    email: String
}
```

### 輸入類型

```graphql
input PostFilter {
    category: String
    authorId: ID
    publishedAfter: DateTime
    tags: [String!]
}

input PaginationInput {
    offset: Int = 0
    limit: Int = 20
}
```

## 解析器（Resolver）

### JavaScript 實現

```javascript
const resolvers = {
    Query: {
        users: () => db.users.findAll(),
        
        user: (parent, { id }, context) => {
            return db.users.findById(id);
        },
        
        posts: (parent, { filter, limit = 10 }, context) => {
            return db.posts.findAll({ ...filter, limit });
        }
    },
    
    Mutation: {
        createUser: (parent, { input }, context) => {
            const user = db.users.create(input);
            return user;
        },
        
        updateUser: (parent, { id, input }, context) => {
            return db.users.update(id, input);
        }
    },
    
    // 巢狀解析器
    User: {
        posts: (user, args, context) => {
            return db.posts.findByAuthorId(user.id);
        }
    }
};
```

### 解析器參數

```javascript
const resolver = {
    // parent: 上層解析器的回傳值
    // args: 查詢參數
    // context: 共享上下文
    // info: 完整的 AST
    field: (parent, args, context, info) => {
        console.log('Parent:', parent);
        console.log('Args:', args);
        console.log('Context:', context);
        return result;
    }
};
```

## 關聯處理

### 一對多關聯

```graphql
type User {
    id: ID!
    posts: [Post!]!
}

type Post {
    id: ID!
    author: User!
}
```

```javascript
const resolvers = {
    User: {
        posts: (user) => db.posts.findByAuthorId(user.id)
    },
    
    Post: {
        author: (post) => db.users.findById(post.authorId)
    }
};
```

### 多對多關聯

```graphql
type Post {
    id: ID!
    tags: [Tag!]!
}

type Tag {
    id: ID!
    name: String!
    posts: [Post!]!
}

type PostTag {
    postId: ID!
    tagId: ID!
}
```

### DataLoader 批次優化

```javascript
import DataLoader from 'dataloader';

// 批次查詢
const userLoader = new DataLoader(ids => {
    return db.users.findByIds(ids);
});

const resolvers = {
    Post: {
        author: (post, args, { userLoader }) => {
            return userLoader.load(post.authorId);
        }
    }
};
```

## 即時架構（Subscriptions）

### 定義

```graphql
type Subscription {
    postCreated: Post!
    postUpdated(id: ID!): Post!
    userJoined(userId: ID!): User!
}
```

### 實現

```javascript
const { PubSub } = require('graphql-subscriptions');

const pubsub = new PubSub();

const resolvers = {
    Subscription: {
        postCreated: {
            subscribe: () => pubsub.asyncIterator(['POST_CREATED'])
        },
        
        postUpdated: {
            subscribe: (root, { id }) => {
                return pubsub.asyncIterator(`POST_UPDATED_${id}`);
            }
        }
    }
};

// 發布事件
async function createPost(input) {
    const post = await db.posts.create(input);
    pubsub.publish('POST_CREATED', { postCreated: post });
    return post;
}
```

### 用戶端

```javascript
import { GraphQLWsLink } from '@apollo/client/link/subscriptions';
import { createClient } from 'graphql-ws';

const client = createClient({
    url: 'ws://localhost:4000/graphql',
});

const link = new GraphQLWsLink(client);

const subscription = `
    subscription OnPostCreated {
        postCreated {
            id
            title
            content
        }
    }
`;

client.subscribe({ query: subscription }, {
    next: (data) => console.log('New post:', data),
    error: (err) => console.error('Error:', err),
    complete: () => console.log('Subscription complete'),
});
```

## 驗證和授權

### 自訂驗證

```javascript
const resolvers = {
    Mutation: {
        createPost: async (parent, { input }, { user }) => {
            if (!user) {
                throw new AuthenticationError('Must be logged in');
            }
            
            if (input.title.length < 5) {
                throw new UserInputError('Title must be at least 5 characters');
            }
            
            return db.posts.create({ ...input, authorId: user.id });
        }
    }
};
```

### 授權裝飾器

```javascript
@Authorized()
async createPost(root, { input }, { user }) {
    // 僅已登入用戶可訪問
}

@Authorized('ADMIN')
async deleteUser(root, { id }, { user }) {
    // 僅管理員可訪問
}

@Authorized((args, { user }) => user.role === 'MODERATOR')
async moderateContent(root, { contentId }, { user }) {
    // 自訂授權邏輯
}
```

### 行級授權

```graphql
type User {
    id: ID!
    email: String!  # 所有用戶可見
    password: String! # 僅本人和管理員可見
    privateData: String @auth(requires: OWNER)
}
```

## 錯誤處理

### GraphQL 錯誤

```javascript
const { GraphQLError } = require('graphql');

throw new GraphQLError(
    'Post not found',  // 訊息
    {
        extensions: {
            code: 'POST_NOT_FOUND',
            field: 'id'
        }
    }
);
```

### 用戶端處理

```javascript
const { ApolloError } = require('@apollo/client');

try {
    const { data } = await apolloClient.mutate({
        mutation: CREATE_POST,
        variables: { input }
    });
} catch (error) {
    if (error.graphQLErrors) {
        error.graphQLErrors.forEach(({ message, locations, path, extensions }) => {
            if (extensions.code === 'POST_NOT_FOUND') {
                showNotification('找不到文章');
            }
        });
    }
    
    if (error.networkError) {
        showNotification('網路錯誤');
    }
}
```

## 片段（Fragments）

### 定義和使用

```graphql
fragment UserFields on User {
    id
    name
    email
    avatarUrl
}

query {
    me {
        ...UserFields
    }
    
    author: user(id: "123") {
        ...UserFields
    }
}
```

### 巢狀片段

```graphql
fragment PostWithAuthor on Post {
    id
    title
    author {
        ...UserFields
    }
}
```

## 內省（Introspection）

```graphql
# 查詢 Schema
{
    __schema {
        types {
            name
            kind
            description
            fields {
                name
                type {
                    name
                    kind
                }
            }
        }
    }
}

# 查詢特定類型
{
    __type(name: "User") {
        name
        fields {
            name
            type {
                name
            }
        }
    }
}
```

## 快取

### Apollo Client 快取

```javascript
import { InMemoryCache } from '@apollo/client';

const cache = new InMemoryCache({
    typePolicies: {
        Query: {
            fields: {
                users: {
                    merge(existing = [], incoming) {
                        return incoming;
                    }
                }
            }
        },
        User: {
            keyFields: ["id"],
            fields: {
                posts: {
                    merge: false  # 不合併，直接替換
                }
            }
        }
    }
});
```

### 快取控制

```graphql
query GetUser($id: ID!) {
    user(id: $id) @cacheControl(maxAge: 300) {
        id
        name
        email
    }
}
```

## 效能優化

### 查詢複雜度限制

```javascript
import { createComplexityLimitRule } from 'graphql-query-complexity';

const complexityRule = createComplexityLimitRule(1000, {
    formatErrorMessage: ({ complexity, limit }) =>
        `複雜度 ${complexity} 超過限制 ${limit}`
});

const server = new ApolloServer({
    schema,
    validationRules: [complexityRule]
});
```

### 深度限制

```javascript
import depthLimit from 'graphql-depth-limit';

const depthRule = depthLimit(10, { ignore: ['_id'] });

const server = new ApolloServer({
    schema,
    validationRules: [depthRule]
});
```

## 工具生態

- **Apollo Server/Client** - GraphQL 伺服器和客戶端
- **Prisma** - 資料庫 ORM 和 GraphQL 整合
- **GraphQL Yoga** - 輕量級 GraphQL 伺服器
- **Hasura** - 自動生成 GraphQL API
- **GraphQL Code Generator** - 從 Schema 生成類型

## 應用場景

- 行動應用（精確資料獲取）
- 微服務聚合層
- 即時更新應用
- 多平台 API

## 相關主題

- [API](API.md) - API 設計基礎
- [RESTful](RESTful.md) - REST API
- [JavaScript](JavaScript.md) - 後端 JavaScript
