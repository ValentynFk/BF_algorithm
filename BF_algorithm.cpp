#include "pch.h" // for precompiled libraries NO NEED!
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <regex>

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

std::vector< std::pair<std::string, std::vector<std::string>> > json2connections(std::string json_str)
{
	std::vector< std::pair<std::string, std::vector<std::string>> > serialized;
	std::regex curved_braces(R"raw(^[{]{1}[ \t]*(.*)[ \t]*[}]{1}$)raw");
	std::regex connections(R"raw("[ \t]*([^"]*)"[ ]*:[ ]*[\[]{1}[ ]*([^\]]*)[ ]*[\]]{1}[,]?[ \t]*)raw");
	std::regex node(R"raw([ \t]*"([^"]*)"[,]?)raw");
	std::smatch captured_parts;
	if (!std::regex_search(json_str, captured_parts, curved_braces)) { return serialized; }
	json_str = captured_parts.str(1);
	std::sregex_iterator end;
	std::sregex_iterator pos(json_str.cbegin(), json_str.cend(), connections);
	for (; pos != end; ++pos) {
		std::vector<std::string> connected_nodes;
		std::string substr(pos->str(2));
		std::sregex_iterator sub_pos(substr.cbegin(), substr.cend(), node);
		for (; sub_pos != end; ++sub_pos) {
			connected_nodes.push_back(sub_pos->str(1));
		}
		serialized.push_back(std::make_pair(pos->str(1), connected_nodes));
	}
	return serialized;
}

int main()
{
	std::vector< std::pair<std::string, std::vector<std::string>> > connections;
	std::vector<std::vector<size_t>> d_matrix;
	std::chrono::system_clock::time_point tp1;
	std::chrono::system_clock::time_point tp2;
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
	std::string json_connections_map(R"raw({  "node1" : ["node2","node3","node4","node5"], "node2" : ["node1","node3"], "node3" : ["node1","node2","node4"], "node4" : ["node1","node3","node5"], "node5" : ["node1","node4","node6"], "node6" : ["node5"]  })raw");
	connections = json2connections(json_connections_map);

	tp1 = std::chrono::system_clock::now();	// set timepoint 1
	d_matrix = BF_algorithm(connections);
	tp2 = std::chrono::system_clock::now();	// set timepoint 2
	for (const auto & row : d_matrix) {
		for (const auto & cell : row) {
			std::cout << cell << '\t';
		}
		std::cout << '\n';
	} std::cout << '\b' << std::endl;
	auto diff = tp2 - tp1;
	std::cout << "time on execution: " << std::chrono::duration_cast<std::chrono::microseconds>(diff).count() << std::endl;
	return EXIT_SUCCESS;
}
