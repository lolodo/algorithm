import edu.princeton.cs.algs4.StdRandom;

import java.util.Iterator;

public class RandomizedQueue<Item> implements Iterable<Item> {
    @SuppressWarnings(value = {"unchecked"})
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
            count++;
        }

        return count;
    }                        // return the number of items on the queue

    public void enqueue(Item item) {
        if (item == null) {
            throw new java.lang.NullPointerException();
        }

        final int size = elems.length;
        if (size == 0) {
            resize(1);
        } else if (size <= numElems) {
            resize(2 * size);
        }

        elems[numElems++] = item;
    }

    private void resize(int max) {
        @SuppressWarnings(value = {"unchecked"})
        final Item[] temp = (Item[]) new Object[max];
        final int arrLength = elems.length;
        int numElemToCopy;

        if (arrLength > max) {
            numElemToCopy = max;
        } else {
            numElemToCopy = arrLength;
        }

        System.arraycopy(elems, 0, temp, 0, numElemToCopy);
        elems = temp;
    }

    public Item dequeue() {
        if (isEmpty()) {
            throw new java.util.NoSuchElementException();
        }

        int rand = StdRandom.uniform(numElems);
        final Item obj = elems[rand];
        elems[rand] = elems[--numElems];

        final int arrLength = elems.length;
        if (numElems > 0 && numElems == arrLength / 4) {
            resize(arrLength / 2);
        }

        return obj;
    }

    public Item sample() {
        if (isEmpty()) {
            throw new java.util.NoSuchElementException();
        }

        Item obj = null;
        while (obj == null) {
            int rand = StdRandom.uniform(numElems);
            obj = elems[rand];
        }

        return obj;
    }

    public Iterator<Item> iterator() {
        return new RandomizedQueueIterator<Item>(elems);
    }

    private static class RandomizedQueueIterator<Item> implements Iterator<Item> {
        private RandomizedQueue<Item> newColl = new RandomizedQueue<Item>();

        public RandomizedQueueIterator(Object[] items) {
            for (Object o : items) {
                if (o == null) {
                    break;
                }
//                @SuppressWarnings(value = {"unchecked"})
                newColl.enqueue((Item) o);
            }
        }

        @Override
        public boolean hasNext() {
            return !newColl.isEmpty();
        }

        @Override
        public Item next() {
            if (newColl.isEmpty()) {
                throw new java.util.NoSuchElementException();
            }

            return newColl.dequeue();
        }

        @Override
        public  void remove() {
            throw new UnsupportedOperationException();
        }
    }
//    public static void main(String[] args)   // unit testing (optional)
}