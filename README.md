# Algorithm to count the number of consistent sub-DAGs in a DAG(Directed Acyclic Graph)

## Usage
To run the program, execute "./graph_alg <FILE> [ROOTID]", where "<FILE>" is the DAG
datafile to be used. If the <FILE> argument isn't given, the default file used is
"./data/Graph/mini-tree.txt". Then "[ROOTID]" is the id of root for the DAG.

## Algorithm

```
// g(G), calculate the number of consistent sub-DAGs in a DAG g
count_consistent_subdag(G, rootid)
{
	modified = copy(G);
	mpnodes = find nodes with multiple parents;
	modified, decomp_subdags = decompose_dag(modified, mpnodes);
	
	for_each(subdag in decomp_subdags)
	{
		num = count_consistent_subdag(subdag, subdag.rootid);
		modified.add_node(subdag.rootid);
		modified.set_node_count(subdag.rootid, num);
	}

	return count_consistent_subdag_for_independent_subdag(modified);
}

//g(G), similar to previous one, but can take multiple roots
count_consistent_subdag(G, rootlist)
{
	//major algorithm are same to previous one
	modified = copy(G);
	mpnodes = find nodes with multiple parents;
	modified, decomp_subdags = decompose_dag(modified, mpnodes);
	
	for_each(subdag in decomp_subdags)
	{
		num = count_consistent_subdag(subdag, subdag.rootid);
		modified.add_node(subdag.rootid);
		modified.set_node_count(subdag.rootid, num);
	}

	return count_consistent_subdag_for_independent_subdag(modified);
}
```

```
decompose_dag(G, mpnodes)
{
	find independent sub-DAG in G,
		//now the independent sub-DAG can have only one root
		//and it can have only one parent in original DAG G
	remove these sub-DAG from G,
	and put these sub-DAGs to a list 'subdags',
	return G, subdags;
}
```


```
// g(G), calculate the number of consistent sub-DAGs in a DAG g
count_consistent_subdag_for_independent_subdag(G)
{
	modified = copy(G);
	mpnodes = find nodes with multiple parents;
	for_each(node in mpnodes)
	{
		modified = remove_subdag_to_become_trees(
					modified, node.id, &extend_subdags);
	}
	
	total = count_consistent_subdag_tree(modified);
	
	// add things back
	while(extend_subdags not empty)
	{
		subdag = extend_subdags.pop();
		
		check dependency for subdag.root;
		if (failed)
		{
			// move to the end of list
			extend_subdags.push_back(subdag);
			continue;
		}
		
		num = count_consistent_subdag_tree(subdag);
		
		modified.add_node(subdag.rootid);
		modified.set_node_count(subdag.rootid, num);
		
		total += count_consistent_subdag_adding_subdag(
					modified, mpnodes, subdag);
	}
	
	return total;
}
```

```
remove_subdag_to_become_trees(&G, rootid, &extend_subdags)
{
	// Pre-order traversal
	fringe = G.getChildList(rootid);

	while (!fringe.empty())
	{
		node = fringe.pop_front();;
		if (node has multiple parents)
		{
			if (node in G)
				remove_subdag_to_become_trees(
						g, node.id, extend_subdags);
		}
		else
		{
			children = G.getChildList(node.id);
			fringe.insert_front(children);
		}
	}
	
	subdag = sub-DAG can be reached from node with id=rootid
	G.remove_subdag(subdag);
	extend_subdags.push_back(subdag)
}
```

```
count_consistent_subdag_tree(tree)
{
	total = 1;
	for_each(sub tree)
	{
		total *= (1 + count_consistent_subdag_tree(sub tree));
	}
	return total;
}
```

```
count_consistent_subdag_adding_subdag(G, subdag)
{
	pathdag = get_path_to_root(G, subdag.rootid);
	return count_consistent_subdag_by_cutting_fixed_path(
			G, subdag.rootid, pathdag);
}
```

```
count_consistent_subdag_by_cutting_fixed_path(G,
		fixed, fixed_path)
{
	modified = copy(G);
	// cut fixed path
	modified.removeSubdag(fixed_path);
	
	rootlist = modified.getRootList();
	
	total = 1; // the case for only the path presenting

	// Check whether there is something left
	if (rootlist.size() != 0)
	{
		total += count_consistent_subdag(
					modified, rootlist);
	}
	
	return total;
}
```


