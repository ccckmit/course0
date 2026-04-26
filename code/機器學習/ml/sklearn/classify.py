import sys
from sklearn.datasets import load_iris, load_breast_cancer, load_wine, load_digits, fetch_covtype
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, classification_report
from sklearn.ensemble import RandomForestClassifier, GradientBoostingClassifier, AdaBoostClassifier
from sklearn.linear_model import LogisticRegression
from sklearn.svm import SVC
from sklearn.neighbors import KNeighborsClassifier
from sklearn.tree import DecisionTreeClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay

DATASETS = {
    "1": ("iris", load_iris),
    "2": ("breast_cancer", load_breast_cancer),
    "3": ("wine", load_wine),
    "4": ("digits (0-9 手寫數字)", load_digits),
    "5": ("covtype (森林覆蓋類型)", fetch_covtype),
}

CLASSIFIERS = {
    "1": ("RandomForestClassifier", RandomForestClassifier(n_estimators=100, random_state=42)),
    "2": ("GradientBoostingClassifier", GradientBoostingClassifier(n_estimators=100, random_state=42)),
    "3": ("AdaBoostClassifier", AdaBoostClassifier(n_estimators=100, random_state=42)),
    "4": ("LogisticRegression", LogisticRegression(max_iter=200, random_state=42)),
    "5": ("SVC", SVC(random_state=42)),
    "6": ("KNeighborsClassifier", KNeighborsClassifier()),
    "7": ("DecisionTreeClassifier", DecisionTreeClassifier(random_state=42)),
    "8": ("GaussianNB", GaussianNB()),
}

def main():
    print("可用資料集:")
    for key, (name, _) in DATASETS.items():
        print(f"  {key}. {name}")

    if len(sys.argv) > 1:
        ds_choice = sys.argv[1]
    else:
        ds_choice = input("\n請選擇資料集: ").strip()

    if ds_choice not in DATASETS:
        print("無效的資料集選擇")
        return

    ds_name, ds_func = DATASETS[ds_choice]
    print(f"\n使用資料集: {ds_name}")

    print("\n可用分類器:")
    for key, (name, _) in CLASSIFIERS.items():
        print(f"  {key}. {name}")

    clf_choice = input("\n請選擇分類器: ").strip()

    if clf_choice not in CLASSIFIERS:
        print("無效的分類器選擇")
        return

    clf_name, clf = CLASSIFIERS[clf_choice]
    print(f"\n使用分類器: {clf_name}")

    ds = ds_func()
    X, y = ds.data, ds.target
    target_names = None
    if hasattr(ds, 'target_names'):
        tn = ds.target_names
        if hasattr(tn, 'dtype') and tn.dtype.kind in ('U', 'O', 'S') and len(tn) == len(set(y)):
            target_names = tn
        elif isinstance(tn, (list, tuple)) and tn and isinstance(tn[0], str) and len(tn) == len(set(y)):
            target_names = tn

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3, random_state=42)

    clf.fit(X_train, y_train)
    y_pred = clf.predict(X_test)

    print(f"\nAccuracy: {accuracy_score(y_test, y_pred):.2f}")
    if target_names is not None:
        print(classification_report(y_test, y_pred, target_names=target_names))
    else:
        print(classification_report(y_test, y_pred))
    # confusion matrix
    cm = confusion_matrix(y_test, y_pred)

    print("\nConfusion Matrix:")
    print(cm)

if __name__ == "__main__":
    main()
