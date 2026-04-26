class Animal:
    def __init__(self, name, sound):
        self.name = name
        self.sound = sound

    def speak(self):
        print(self.name, "says", self.sound)

class Dog(Animal):
    def __init__(self, name):
        self.name = name
        self.sound = "Woof"

    def fetch(self):
        print(self.name, "fetches the ball!")

dog = Dog("Rex")
dog.speak()
dog.fetch()

cat = Animal("Kitty", "Meow")
cat.speak()
