public class RandomizedQueue<Item> implements Iterable<Item> {
    private  Item[] elems = (Item[]) new Object[0];
    private  int numElems = 0;

    public RandomizedQueue(){

    }                 // construct an empty randomized queue

    public boolean isEmpty() {
        return numElems == 0;
    }             // is the queue empty?

    public int size() {
        if (isEmpty()) {
            return 0;
        }

        int count = 0;
        for (Item i:this) {

        }
    }                        // return the number of items on the queue
    public void enqueue(Item item)           // add the item
    public Item dequeue()                    // remove and return a random item
    public Item sample()                     // return (but do not remove) a random item
    public Iterator<Item> iterator()         // return an independent iterator over items in random order
    public static void main(String[] args)   // unit testing (optional)
}