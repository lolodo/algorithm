
def quick_sort(arr):
    """sort method for more"""
    if len(arr) < 2:
        return arr
    else:
        pivot = arr[0]
        less = []
        greater = []
        for i in arr[1:]:
            if i <= pivot:
                less.append(i)
            else:
                greater.append(i)
        return quick_sort(less) + [pivot] + quick_sort(greater)


if __name__ == '__main__':
    ARRAY = [4, 2, -4, 5, 2, -12, 56, 23, 67]
    print ARRAY
    NEW_ARR = quick_sort(ARRAY)
    print NEW_ARR
