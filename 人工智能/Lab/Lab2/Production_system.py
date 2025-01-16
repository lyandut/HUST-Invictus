from collections import OrderedDict

# 定义规则集
RULES = [
    {"if": {"毛发": True}, "then": "哺乳动物"},
    {"if": {"奶": True}, "then": "哺乳动物"},
    {"if": {"羽毛": True}, "then": "鸟"},
    {"if": {"飞": True, "下蛋": True}, "then": "鸟"},
    {"if": {"吃肉": True}, "then": "食肉动物"},
    {"if": {"锋利牙齿": True, "锋利爪子": True, "眼盯前方": True}, "then": "食肉动物"},
    {"if": {"哺乳动物": True, "有蹄": True}, "then": "有蹄类动物"},
    {"if": {"哺乳动物": True, "反刍动物": True}, "then": "有蹄类动物"},
    {"if": {"哺乳动物": True, "食肉动物": True, "黄褐色": True, "暗斑点":True}, "then": "金钱豹"},
    {"if": {"哺乳动物": True, "食肉动物": True, "黄褐色": True, "黑色条纹": True}, "then": "虎"},
    {"if": {"有蹄类动物": True, "长脖子": True, "长腿": True, "暗斑点": True}, "then": "长颈鹿"},
    {"if": {"有蹄类动物": True, "黑色条纹": True}, "then": "斑马"},
    {"if": {"鸟": True, "长脖子": True, "长腿": True, "不会飞": True, "黑白二色": True}, "then": "鸵鸟"},
    {"if": {"鸟": True, "游泳": True, "长腿": True, "不会飞": True, "黑白二色": True}, "then": "企鹅"},
    {"if": {"鸟": True, "善飞": True}, "then": "信天翁"},
]
FeaturesList = []
AnimalFeatures = {}

def printRule():
    print("在以下特征中，选取动物特征（输入特征前面的序号，每行输入一个特征，空行表示输入结束）：")
    features = []
    for rule in RULES:
        for key, _ in rule["if"].items():
            features.append(key)

    global FeaturesList
    FeaturesList = list(OrderedDict.fromkeys(features))
    for i, key in enumerate(FeaturesList, 1):
        print(f"{i}: {key}", end = " ")
        if i != 0 and i % 8 == 0:
            print()

def getFeature():
    global FeaturesList
    global AnimalFeatures

    for key in FeaturesList:
        AnimalFeatures[key] = False

    line = input()
    while line:
        AnimalFeatures[FeaturesList[int(line) - 1]] = True
        line = input()

# 推理机函数
def inferAnimal():
    for rule in RULES:
        if all(rule["if"].get(key)  == value for key, value in AnimalFeatures.items() if value == True):
            return rule["then"]
    return "未知动物"

# 输出全部特征
printRule()

# 获取输入特征
getFeature()

# 识别动物
animal = inferAnimal()
print(f"识别出的动物是：{animal}")