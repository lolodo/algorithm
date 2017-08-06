public class MammalInterface implements AnimalInterface {
    public void eat() {
        System.out.println("Mammal eats!");
    }

    public void travel() {
        System.out.println("Mammal travels!");
    }

    public int noOflegs() {
        return 0;
    }

    public static void main(String[] args) {
        MammalInterface m = new MammalInterface();
        m.eat();
        m.travel();
    }
}