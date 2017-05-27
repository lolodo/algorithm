def find_best_stations(needed, states):
    all_names = set()
    while needed:
        joint = set()
        saved = set()
        names = None
        for name, serial in states.items():
            joint = serial & needed
            if len(joint) > len(saved):
                saved = joint
                names = name
        
        if names != None:
            all_names.add(names)
            needed -=saved

    return all_names
                

if __name__ == '__main__':
    states_needed = set(["mt", "wa", "or", "id", "nv", "ut","ca", "az"])
    stations = {}
    stations["kone"] = set(["id", "nv", "ut"])
    stations["ktwo"] = set(["wa", "id", "mt"])
    stations["kthree"] = set(["or", "nv", "ca"])
    stations["kfour"] = set(["nv", "ut"])
    stations["kfive"] = set(["ca", "az"])

    # final_stations = set()
    # while states_needed:
    #     best_station = None
    #     states_covered = set()

    #     for station, states_for_station in stations.items():
    #         covered = states_needed & states_for_station
    #         if len(covered) > len(states_covered):
    #             best_station = station
    #             states_covered = covered
            
    #     final_stations.add(best_station)
    #     states_needed -= states_covered

    result = find_best_stations(states_needed, stations)
    print(result)


