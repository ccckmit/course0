"""
前處理法範例
展示各種資料前處理技術
"""

import math
import random
from typing import List, Tuple, Dict
from collections import defaultdict, Counter


class Preprocessing:
    """前處理方法"""
    
    @staticmethod
    def normalize(arr: List[float]) -> List[float]:
        """最小最大正規化: (x - min) / (max - min) -> [0, 1]"""
        min_val, max_val = min(arr), max(arr)
        if max_val == min_val:
            return [0.5] * len(arr)
        return [(x - min_val) / (max_val - min_val) for x in arr]
    
    @staticmethod
    def standardize(arr: List[float]) -> List[float]:
        """Z-score 標準化: (x - mean) / std"""
        mean = sum(arr) / len(arr)
        std = math.sqrt(sum((x - mean) ** 2 for x in arr) / len(arr))
        if std == 0:
            return [0] * len(arr)
        return [(x - mean) / std for x in arr]
    
    @staticmethod
    def log_transform(arr: List[float]) -> List[float]:
        """對數變換: log(x + 1)"""
        return [math.log(x + 1) for x in arr]
    
    @staticmethod
    def robust_scale(arr: List[float]) -> List[float]:
        """強韌縮放: 使用中位數"""
        sorted_arr = sorted(arr)
        n = len(sorted_arr)
        median = sorted_arr[n // 2]
        
        q1 = sorted_arr[n // 4]
        q3 = sorted_arr[3 * n // 4]
        iqr = q3 - q1
        
        if iqr == 0:
            return [0] * len(arr)
        return [(x - median) / iqr for x in arr]


class DataCleaning:
    """資料清洗"""
    
    @staticmethod
    def remove_outliers(arr: List[float], threshold: float = 2.0) -> List[float]:
        """移除異常值 (Z-score 方法)"""
        mean = sum(arr) / len(arr)
        std = math.sqrt(sum((x - mean) ** 2 for x in arr) / len(arr))
        
        if std == 0:
            return arr
        
        return [x for x in arr if abs((x - mean) / std) <= threshold]
    
    @staticmethod
    def handle_missing(values: List, strategy: str = 'mean') -> List:
        """處理缺失值"""
        values = list(values)
        missing_indices = [i for i, v in enumerate(values) if v is None]
        
        if not missing_indices:
            return values
        
        valid_values = [v for v in values if v is not None]
        
        if strategy == 'mean' and valid_values:
            fill_value = sum(valid_values) / len(valid_values)
        elif strategy == 'median' and valid_values:
            sorted_vals = sorted(valid_values)
            fill_value = sorted_vals[len(sorted_vals) // 2]
        elif strategy == 'mode' and valid_values:
            fill_value = Counter(valid_values).most_common(1)[0][0]
        elif strategy == 'zero':
            fill_value = 0
        else:
            fill_value = None
        
        for i in missing_indices:
            values[i] = fill_value
        
        return values
    
    @staticmethod
    def remove_duplicates(arr: List) -> List:
        """移除重複項"""
        seen = set()
        result = []
        for item in arr:
            if item not in seen:
                seen.add(item)
                result.append(item)
        return result


class FeatureEngineering:
    """特徵工程"""
    
    @staticmethod
    def one_hot_encode(categories: List[str]) -> Tuple[Dict, List[List[int]]]:
        """One-Hot 編碼"""
        unique_cats = sorted(set(categories))
        cat_to_idx = {cat: i for i, cat in enumerate(unique_cats)}
        
        encoded = []
        for cat in categories:
            row = [0] * len(unique_cats)
            row[cat_to_idx[cat]] = 1
            encoded.append(row)
        
        return cat_to_idx, encoded
    
    @staticmethod
    def label_encode(categories: List[str]) -> Dict[str, int]:
        """標籤編碼"""
        unique_cats = sorted(set(categories))
        return {cat: i for i, cat in enumerate(unique_cats)}
    
    @staticmethod
    def binning(values: List[float], n_bins: int = 5) -> List[int]:
        """分箱"""
        min_val, max_val = min(values), max(values)
        if min_val == max_val:
            return [0] * len(values)
        
        bin_size = (max_val - min_val) / n_bins
        return [min(int((v - min_val) / bin_size), n_bins - 1) for v in values]
    
    @staticmethod
    def polynomial_features(arr: List[float], degree: int = 2) -> List[List[float]]:
        """多項式特徵"""
        result = []
        for x in arr:
            features = [1]
            for d in range(1, degree + 1):
                features.append(x ** d)
            result.append(features)
        return result


class DataTransformation:
    """資料轉換"""
    
    @staticmethod
    def discretize(values: List[float], boundaries: List[float]) -> List[int]:
        """離散化"""
        return [sum(b <= v for b in boundaries) for v in values]
    
    @staticmethod
    def clip(values: List[float], min_val: float, max_val: float) -> List[float]:
        """裁剪"""
        return [max(min_val, min(max_val, v)) for v in values]
    
    @staticmethod
    def square_transform(arr: List[float]) -> List[float]:
        """平方變換"""
        return [x ** 2 for x in arr]
    
    @staticmethod
    def sqrt_transform(arr: List[float]) -> List[float]:
        """平方根變換"""
        return [math.sqrt(max(0, x)) for x in arr]


def demo_normalization():
    """正規化範例"""
    print("=" * 50)
    print("1. 正規化方法")
    print("=" * 50)
    
    arr = [10, 20, 30, 40, 50]
    
    print(f"\n原始資料: {arr}")
    print(f"最小最大正規化: {Preprocessing.normalize(arr)}")
    print(f"標準化: {[round(x, 2) for x in Preprocessing.standardize(arr)]}")
    print(f"對數變換: {[round(x, 2) for x in Preprocessing.log_transform(arr)]}")


def demo_data_cleaning():
    """資料清洗範例"""
    print("\n" + "=" * 50)
    print("2. 資料清洗")
    print("=" * 50)
    
    arr = [1, 2, 100, 3, 4, 200, 5, 6]
    print(f"\n原始資料: {arr}")
    print(f"移除異常值 (threshold=2.0): {DataCleaning.remove_outliers(arr)}")
    
    values = [1, 2, None, 4, None, 6]
    print(f"\n缺失值處理: {values}")
    print(f"  用平均填充: {DataCleaning.handle_missing(values, 'mean')}")
    
    dupes = [1, 2, 3, 2, 1, 4, 3, 5]
    print(f"\n移除重複: {dupes} -> {DataCleaning.remove_duplicates(dupes)}")


def demo_feature_engineering():
    """特徵工程範例"""
    print("\n" + "=" * 50)
    print("3. 特徵工程")
    print("=" * 50)
    
    cats = ['cat', 'dog', 'cat', 'bird', 'dog', 'cat']
    
    mapping, encoded = FeatureEngineering.one_hot_encode(cats)
    print(f"\nOne-Hot 編碼: {cats}")
    print(f"  映射: {mapping}")
    print(f"  編碼結果: {encoded}")
    
    print(f"\n標籤編碼: {cats}")
    print(f"  {FeatureEngineering.label_encode(cats)}")
    
    values = [1.2, 3.5, 5.7, 8.9, 10.1]
    print(f"\n分箱 (5 bins): {values}")
    print(f"  {FeatureEngineering.binning(values, 5)}")
    
    print(f"\n多項式特徵 (degree=2): {values[0:3]}")
    print(f"  {FeatureEngineering.polynomial_features(values[0:3], 2)}")


def demo_data_transformation():
    """資料轉換範例"""
    print("\n" + "=" * 50)
    print("4. 資料轉換")
    print("=" * 50)
    
    values = [-5, 0, 5, 10, 15, 20]
    
    print(f"\n原始: {values}")
    print(f"離散化 (boundaries=[0,10,20]): {DataTransformation.discretize(values, [0, 10, 20])}")
    print(f"裁剪 (0, 15): {DataTransformation.clip(values, 0, 15)}")
    print(f"平方: {DataTransformation.square_transform(values)}")
    print(f"平方根: {[round(x, 2) for x in DataTransformation.sqrt_transform(values)]}")


def demo_robust_scaling():
    """強韌縮放範例"""
    print("\n" + "=" * 50)
    print("5. 強韌縮放")
    print("=" * 50)
    
    arr = [1, 2, 3, 4, 5, 6, 7, 8, 9, 100]
    
    print(f"\n原始 (含異常值): {arr}")
    print(f"標準化: {[round(x, 2) for x in Preprocessing.standardize(arr)]}")
    print(f"強韌縮放: {[round(x, 2) for x in Preprocessing.robust_scale(arr)]}")


if __name__ == "__main__":
    print("前處理法 Python 範例")
    print("=" * 50)
    
    demo_normalization()
    demo_data_cleaning()
    demo_feature_engineering()
    demo_data_transformation()
    demo_robust_scaling()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
