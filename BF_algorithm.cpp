#include "pch.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

size_t distance(
	const std::string & current_node, const std::string & node2,
	const std::vector< std::pair<std::string, std::vector<std::string>> > & connections,
	const size_t & m_depth, size_t depth = 0,
	std::vector<std::string> passed_nodes = std::vector<std::string>())
{
	if (current_node == node2) { return depth; }	// resulting node reached :D
	++depth;	// next iteration means deeper state
	if (depth > m_depth) { return m_depth + 1; }	// out of bounds, surrender
	if (std::find(passed_nodes.cbegin(), passed_nodes.cend(), current_node) != passed_nodes.cend()) {
		return m_depth + 1;
	}
	else passed_nodes.push_back(current_node);	// check against passing excessive nodes
	auto current_node_iterator = std::find_if(connections.cbegin(), connections.cend(),
		[current_node](const std::pair<std::string, std::vector<std::string>> & node) {
		return (node.first == current_node) ? true : false;
	});	// get vector of connected nodes to the current node
	size_t calculated_depth = m_depth + 1;	// let's look what minimal depth can it reach
	std::for_each(current_node_iterator->second.cbegin(), current_node_iterator->second.cend(),
		[node2, connections, m_depth, depth, &passed_nodes, &calculated_depth](std::string next_node) {
		size_t tmp = distance(next_node, node2, connections, m_depth, depth, passed_nodes);
		if (tmp < calculated_depth) { calculated_depth = tmp; };
	});	// find out the way with minimal resulting depth, remember that depth
	return calculated_depth;
}

std::vector<std::vector<size_t>> BF_algorithm(
	std::vector< std::pair<std::string, std::vector<std::string>> > connections, 
	size_t depth = 10)
{
	std::vector<std::vector<size_t>> d_matrix;	// distances matrix
	std::vector<std::string> n_set;				// set of nodes (node1, node2, node3... nodeN)
	std::for_each(connections.cbegin(), connections.cend(), 
		[&n_set](std::pair<std::string, std::vector<std::string>> node) {
			n_set.push_back(node.first);
		});	// fill the set with nodes
	std::for_each(connections.cbegin(), connections.cend(), 
		[&d_matrix, n_set, connections, depth](const std::pair<std::string, std::vector<std::string>> & node) {
			d_matrix.push_back({});	// create new row
			std::for_each(n_set.cbegin(), n_set.cend(), 
				[&d_matrix, connections, node, depth](const std::string & to_node) {
					d_matrix.back().push_back(distance(node.first, to_node, connections, depth));	// calculate the distance between two nodes
				});	// fill separate row of distances matrix column by column
		});	// fill distances matrix row by row
	return d_matrix;
}

int main()
{
	std::vector< std::pair<std::string, std::vector<std::string>> > connections;
	/* Next fill the connections map with actual connections in a form { 'node' -> [nodes, connected to it] }
	{ 
	  "node1" : ["node2", "node3", "node4", "node5"],
	  "node2" : ["node1", "node3"],
	  "node3" : ["node1", "node2", "node4"],
	  "node4" : ["node1", "node3", "node5"],
	  "node5" : ["node1", "node4", "node6"],
	  "node6" : ["node5"],
	}
	Just like in JSON, instead of node names there can be IPv6 addresses or something else.
	*/
	connections.push_back(std::make_pair(std::string("1"), std::vector<std::string>({ std::string("2"), std::string("3"), std::string("4"), std::string("5") })));
	connections.push_back(std::make_pair(std::string("2"), std::vector<std::string>({ std::string("1"), std::string("3") })));
	connections.push_back(std::make_pair(std::string("3"), std::vector<std::string>({ std::string("1"), std::string("2"), std::string("4") })));
	connections.push_back(std::make_pair(std::string("4"), std::vector<std::string>({ std::string("1"), std::string("3"), std::string("5") })));
	connections.push_back(std::make_pair(std::string("5"), std::vector<std::string>({ std::string("1"), std::string("4"), std::string("6") })));
	connections.push_back(std::make_pair(std::string("6"), std::vector<std::string>({ std::string("5") })));
	for (const auto & row : BF_algorithm(connections)) {
		for (const auto & cell : row) {
			std::cout << cell << '\t';
		}
		std::cout << '\n';
	} std::cout << '\b' << std::endl;
}
