'''
    search queue
'''
from collections import deque

def person_is_seller(name):
    if name[-1] == 'm':
        return True
    else:
        return False

def search(name):
    '''
    search queue
    '''
    search_queue = deque()
    search_queue += graph[name]
    searched = []

    # print search_queue
    # while search

    while search_queue:
        person = search_queue.popleft()
        if person not in searched:
            if person_is_seller(person):
                print person + " is a mango seller!"
                return True
            else:
                search_queue += graph[person]
                searched.append(person)

    return False

if __name__ == '__main__':
    graph = {}
    graph["you"] = ["alice", "bob", "claire"]
    graph["bob"] = ["anuj", "peggy"]
    graph["alice"] = ["peggy"]
    graph["claire"] = ["thom", "jonny"]
    graph["anuj"] = []
    graph["peggy"] = []
    graph["thom"] = []
    graph["jonny"] = []
    search("you")


