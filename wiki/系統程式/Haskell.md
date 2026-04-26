# Haskell

## 概述

Haskell 是標準化、純函數式的程式語言，以數學家 Haskell Curry 命名。Haskell 的設計始於 1987 年，當時一群研究者希望創建一個統一的函數式語言標準。1999 年，Haskell 98 標準發布，成為最廣泛使用的 Haskell 版本。此後，Haskell 持續發展，吸引了學術界和產業界的廣泛關注。

Haskell 的核心特點是純函數性——函數沒有副作用，相同的輸入總是產生相同的輸出。這種設計帶來了眾多好處：程式更容易推理、測試、除錯和並行化。Haskell 還配備了強大的類型系統，包括代數資料類型、模式匹配、類別（Typeclass）等特性，讓程式師能夠在編譯時發現大量錯誤。惰性求值（Laziness）是另一個重要特性，它允許處理無限資料結構，實現了高度抽象的程式設計風格。

## 歷史背景

1987 年，函數式語言研究者聚會並決定創建一個統一的函數式語言標準，以解決當時語言混亂的問題。他們以 Haskell 98 為目標，設計了一個純函數式、惰性求值的語言。1999 年，Haskell 98 語言和函式庫標準正式發布。

2000 年代，Haskell 在學術界和產業界都獲得了廣泛關注。2006 年，Glasgow Haskell Compiler（GHC）成為最流行的 Haskell 實現，支援大量擴展。2009 年，Stackage 上線，提供穩定的套件生態系統。2010 年代，Haskell 在金融、區塊鏈、編譯器等領域獲得應用。2020 年，Haskell 基金會成立，致力於推動 Haskell 的發展。

## 核心概念

### 函數與表達式

```haskell
-- 簡單函數
add :: Int -> Int -> Int
add x y = x + y

-- lambda 表達式
add' = \x y -> x + y

-- 柯里化
curriedAdd :: Int -> (Int -> Int)
curriedAdd = \x -> \y -> x + y

-- 函數組合
compose :: (b -> c) -> (a -> b) -> (a -> c)
compose f g = \x -> f (g x)

-- 中綴運算
(++) :: [a] -> [a] -> [a]
(++) [] ys = ys
(++) (x:xs) ys = x : (xs ++ ys)

-- 區段運算子
add10 = (10 +)
square = (^2)
```

### 代數資料類型

```haskell
-- 枚舉類型
data Bool = False | True

-- 記錄類型
data Point = Point { x :: Double, y :: Double }

-- 參數化類型
data Maybe a = Nothing | Just a

data Either a b = Left a | Right b

data Tree a = Leaf a | Node (Tree a) (Tree a)

-- 使用模式匹配
safeDiv :: Double -> Double -> Maybe Double
safeDiv _ 0 = Nothing
safeDiv x y = Just (x / y)

-- 遞迴資料結構
length' :: [a] -> Int
length' [] = 0
length' (_:xs) = 1 + length' xs

reverse' :: [a] -> [a]
reverse' [] = []
reverse' (x:xs) = reverse' xs ++ [x]

-- 尾遞迴優化
reverse'' :: [a] -> [a]
reverse'' xs = go xs []
  where
    go [] acc = acc
    go (x:xs) acc = go xs (x:acc)
```

### 類別（Typeclass）

```haskell
-- 定義類別
class Eq a where
    (==) :: a -> a -> Bool
    (/=) :: a -> a -> Bool

-- 實作類別
instance Eq Bool where
    True == True = True
    False == False = True
    _ == _ = False
    x /= y = not (x == y)

-- 標準類別
-- Eq: 相等性
-- Ord: 順序
-- Show: 轉字串
-- Read: 解析字串
-- Num: 數值運算
-- Functor: 映射
-- Applicative: 應用
-- Monad: .bind
```

### IO 操作

```haskell
-- IO 類型：與外界互動的動作
main :: IO ()
main = do
    putStrLn "What's your name?"
    name <- getLine
    putStrLn $ "Hello, " ++ name ++ "!"

-- 讀取檔案
readFile' :: FilePath -> IO String
readFile' path = do
    content <- readFile path
    return (lines content)

-- 命令列參數
import System.Environment (getArgs)

main' :: IO ()
main' = do
    args <- getArgs
    case args of
        [input, output] -> processFile input output
        _ -> putStrLn "Usage: program input output"

-- 例外處理
import Control.Exception (try, SomeException)

safeRead :: Read a => String -> IO (Maybe a)
safeRead s = case reads s of
    [(x, "")] -> return (Just x)
    _ -> return Nothing
```

### 惰性求值

```haskell
-- 無限列表
ones :: [Int]
ones = 1 : ones

nats :: [Int]
nats = 0 : map (+1) nats

fibs :: [Int]
fibs = 0 : 1 : zipWith (+) fibs (tail fibs)

-- 惰性模式匹配
head' :: [a] -> a
head' (x:_) = x

take' :: Int -> [a] -> [a]
take' n (x:xs) | n > 0 = x : take' (n-1) xs
take' _ _ = []

-- 短路求值
and' :: Bool -> Bool -> Bool
and' True True = True
and' _ _ = False

-- 條件表達式（不是語句）
max' :: Ord a => a -> a -> a
max' x y = if x > y then x else y

-- 使用惰性求值的經典例子
-- 計算第 1000 個質數（只計算需要的部分）
primes :: [Int]
primes = sieve [2..]
  where
    sieve (p:xs) = p : sieve [x | x <- xs, x `mod` p /= 0]

nthPrime :: Int -> Int
nthPrime n = primes !! (n - 1)
```

## 函數式模式

### Functor 與 Applicative

```haskell
-- Functor: 可映射的容器
class Functor f where
    fmap :: (a -> b) -> f a -> f b

-- List
instance Functor [] where
    fmap f [] = []
    fmap f (x:xs) = f x : fmap f xs

-- Maybe
instance Functor Maybe where
    fmap _ Nothing = Nothing
    fmap f (Just x) = Just (f x)

-- Applicative: 支持函數的容器
class Functor f => Applicative f where
    pure :: a -> f a
    (<*>) :: f (a -> b) -> f a -> f b

-- 使用 Applicative
addMaybe :: Maybe Int -> Maybe Int -> Maybe Int
addMaybe mx my = (+) <$> mx <*> my

-- 序列操作
sequenceA' :: Applicative f => [f a] -> f [a]
sequenceA' [] = pure []
sequenceA' (x:xs) = (:) <$> x <*> sequenceA' xs
```

### Monad

```haskell
-- Monad: 鏈式操作
class Applicative m => Monad m where
    (>>=) :: m a -> (a -> m b) -> m b
    return :: a -> m a

-- Maybe Monad
instance Monad Maybe where
    Nothing >>= _ = Nothing
    (Just x) >>= f = f x
    return = Just

-- 使用 do 語法
lookup' :: Eq k => k -> [(k, v)] -> Maybe v
lookup' key = go
  where
    go [] = Nothing
    go ((k,v):rest) = if key == k then Just v else go rest

-- 鏈式調用
processData :: String -> Maybe Int
processData s = do
    trimmed <- return (trim s)
    num <- readMaybe trimmed
    guard (num > 0)
    return num

-- List Monad
instance Monad [] where
    xs >>= f = concat (map f xs)
    return x = [x]

-- 列表推導
combinations :: Int -> Int -> [(Int, Int)]
combinations n m = do
    x <- [1..n]
    y <- [1..m]
    return (x, y)
```

### 常用模式

```haskell
-- 折疊
foldl' :: (b -> a -> b) -> b -> [a] -> b
foldl' _ acc [] = acc
foldl' f acc (x:xs) = let acc' = f acc x in seq acc' (foldl' f acc' xs)

foldr :: (a -> b -> b) -> b -> [a] -> b
foldr _ acc [] = acc
foldr f acc (x:xs) = f x (foldr f acc xs)

-- 映射
map' :: (a -> b) -> [a] -> [b]
map' f = foldr (\x acc -> f x : acc) []

-- 過濾
filter' :: (a -> Bool) -> [a] -> [a]
filter' p = foldr (\x acc -> if p x then x : acc else acc) []

-- 遍歷
traverse' :: Applicative f => (a -> f b) -> [a] -> f [b]
traverse' _ [] = pure []
traverse' f (x:xs) = (:) <$> f x <*> traverse' f xs
```

## 實際應用

### 編譯器與語言處理

```haskell
-- 簡單的表達式解釋器
data Expr = Lit Int
          | Add Expr Expr
          | Mul Expr
          | Var String
          | Let String Expr Expr

eval :: [(String, Int)] -> Expr -> Int
eval env (Lit n) = n
eval env (Add e1 e2) = eval env e1 + eval env e2
eval env (Mul e) = eval env e * 2
eval env (Var x) = case lookup x env of
    Just n -> n
    Nothing -> error ("undefined: " ++ x)
eval env (Let x e1 e2) = eval ((x, eval env e1):env) e2

-- 解析器組合器
import Text.Parsec

expr :: Parser Int
expr = do
    spaces
    term `chainl1` (do { spaces; char '+'; spaces; return (+) })

term :: Parser Int
term = do
    spaces
    factor `chainl1` (do { spaces; char '*'; spaces; return (*) })

factor :: Parser Int
factor = do
    spaces
    (number >>= return . read)
    <|> between (char '(') (char ')') expr
```

### 金融計算

```haskell
-- 簡單的 Black-Scholes
blackScholes :: Double -> Double -> Double -> Double -> Double -> Double
blackScholes s k r t v =
    let d1 = (log (s/k) + (r + v^2/2)*t) / (v * sqrt t)
        d2 = d1 - v * sqrt t
        n x = 0.5 * (1 + erf (x / sqrt 2))
    in s * n d1 - k * exp (-r*t) * n d2

-- 投資組合價值
portfolioValue :: [(String, Double, Double)] -> Double -> Double
portfolioValue positions price = sum $ map value position
  where
    value (ticker, quantity, strike)
        | price > strike = quantity * (price - strike)
        | otherwise = 0
```

### 網路請求

```haskell
import Network.HTTP.Simple

-- 簡單 HTTP 請求
getURL :: String -> IO (Response ByteString)
getURL url = httpLBS (parseRequest_ url)

-- 解析 JSON
import Data.Aeson

data User = User { userId :: Int, userName :: String }

instance FromJSON User where
    parseJSON = withObject "User" $ \o ->
        User <$> o .: "id" <*> o .: "name"

fetchUsers :: IO [User]
fetchUsers = do
    response <- getURL "https://api.example.com/users"
    return $ eitherDecode (getResponseBody response)
```

## 工具與生態

### GHC 編譯器

Glasgow Haskell Compiler（GHC）是最流行的 Haskell 實現，提供高效的編譯和豐富的語言擴展。GHCi 是互動式的 REPL，可以用於實驗和除錯。

```bash
# 編譯
ghc -O2 program.hs

# 互動模式
ghci

# 執行
./program

# 優化選項
ghc -O2 -threaded -rtsopts program.hs

# 性能分析
./program +RTS -s
```

### Stackage 與 Cabal

Stackage 提供經過測試的穩定套件版本。Cabal 是 Haskell 的套件管理工具，可以構建和分發 Haskell 專案。

```bash
# 初始化專案
cabal init

# 建構
cabal build

# 測試
cabal test

# 安裝依賴
cabal install
```

## 遺產與影響

Haskell 對函數式編程的發展產生了深遠影響。它的類型系統、惰性求值、Monad 等概念被許多語言借鑒。Scala、F#、Elm、PureScript 等語言都深受 Haskell 影響。

Haskell 在編譯器設計、形式化驗證、金融計算等領域有重要應用。Facebook 使用 Haskell 進行垃圾訊息過濾。GitHub 使用 Haskell 開發部分服務。众多区块链项目选择 Haskell 作为开发语言。

## 相關概念

- [函數式編程](函數式編程.md) - 函數式編程範式
- [Lambda演算](Lambda演算.md) - Haskell 的理論基礎

## Tags

#Haskell #函數式編程 #GHC #Monad #類型系統