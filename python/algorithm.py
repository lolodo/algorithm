
def find_smallest(arr):
    '''
    doc string for findSmallest
    '''
    smallest = arr[0]
    smallest_index = 0
    for i in range(1, len(arr)):
        if arr[i] < smallest:
            smallest = arr[i]
            smallest_index = i
    return smallest_index


def find_biggest(arr):
    '''
    find the biggest member
    '''
    max_val = arr[0]
    max_idx = 0

    for i in range(1, len(arr)):
        if arr[i] > max_val:
            max_val = arr[i]
            max_idx = i

    return max_idx

def selection_sort(arr):
    '''
    selection sort
    '''
    s_idx = 0
    tmp = 0

    for i in range(0, len(arr)):
        s_idx = find_smallest(arr[i:])
        tmp = arr[i]
        arr[i] = arr[s_idx + i]
        arr[s_idx + i] = tmp

    print arr

def sel_sort(arr):
    '''
    sort
    '''
    # print arr
    new_arr = []
    for i in range(0, len(arr)):
        idx = find_smallest(arr)
        new_arr.append(arr.pop(idx))

    return new_arr



def test():
    '''
    Just for testing.
    '''
    array = [2, 3, 5, 1, 9, 3, 7, 5]

    # print array
    # selection_sort(array)
    print array
    new = sel_sort(array)
    print new

if __name__ == '__main__':
    test()
