#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mtest.h"
#include "delivery_algorithm.h"
#include "delivery_sim.h"
#include "resource_calculation.h"
#include "create_routes.h"
#include "astar_helper_functions.h"

//
// TEST delivery_sim.h
//

TEST_CASE(test_generate_random_package, {
    srand(time(NULL));

    for (int i = 0; i < 250; i++) {
        package_t *package = generate_random_package();

        CHECK_TRUE(package->height > 0 && package->height <= 2);
        CHECK_TRUE(package->width > 0 && package->width <= 2);
        CHECK_TRUE(package->length > 0 && package->length <= 2);
        CHECK_TRUE(package->weight > 0 && package->weight <= 25);
        CHECK_TRUE(package->priority >= 1 && package->priority <= 5);
        CHECK_TRUE(package->node_id >= 1 && package->node_id <= 5);
        CHECK_TRUE(package->truck_id >= 1 && package->truck_id <= 5);
        free(package);
    }
})

TEST_CASE(test_generate_random_node, {
    for (int i = 0; i < 250; i++) {
        node_t *node = generate_random_node();

        CHECK_TRUE(node->location_x >= 0 && node->location_x < 100);
        CHECK_TRUE(node->location_y >= 0 && node->location_y < 100);
        CHECK_TRUE(node->id == 0);
        free(node);
    }
})

TEST_CASE(test_generate_random_graph, {
    for (int i = 0; i < 250; i++) {
        graph_t *graph = generate_random_graph(15);
        
        CHECK_TRUE(graph->nodes == 15);
        CHECK_TRUE(graph->adj_matrix != NULL);
        free_matrix(graph);
        free(graph);
    }
})

TEST_CASE(test_create_truck, {
    int truck_id = 1;

    truck_t *truck = create_truck(truck_id);

    CHECK_TRUE(truck->id == truck_id);
    CHECK_TRUE(truck->max_weight == 3500);
    CHECK_TRUE(truck->average_speed == 60);
    CHECK_TRUE(truck->max_volume == 19);
    free(truck);
})

TEST_CASE(test_create_package, {
    int package_priority = 1;
    int node_id = 2;
    int truck_id = 3;
    int height = 4;
    int width = 5;
    int length = 6;
    int weight = 7;

    package_t *package = create_package(package_priority, node_id, truck_id, height, width, length, weight);

    CHECK_TRUE(package->priority == package_priority);
    CHECK_TRUE(package->node_id == node_id);
    CHECK_TRUE(package->truck_id == truck_id);
    CHECK_TRUE(package->height == height);
    CHECK_TRUE(package->width == width);
    CHECK_TRUE(package->length == length);
    CHECK_TRUE(package->weight == weight);
    free(package);
})

TEST_CASE(test_create_node, {
    int location_x = 1;
    int location_y = 2;
    int id = 3;

    node_t *node = create_node(location_x, location_y, id);

    CHECK_TRUE(node->location_x == location_x);
    CHECK_TRUE(node->location_y == location_y);
    CHECK_TRUE(node->id == id);

    // Check if packages are initialized
    for (int i = 0; i < 30; i++) {
        CHECK_TRUE(node->packages[i].priority == -1);
        CHECK_TRUE(node->packages[i].node_id == -1);
        CHECK_TRUE(node->packages[i].truck_id == -1);
        CHECK_TRUE(node->packages[i].height == 0);
        CHECK_TRUE(node->packages[i].width == 0);
        CHECK_TRUE(node->packages[i].length == 0);
        CHECK_TRUE(node->packages[i].weight == 0);
    }
    free(node);
})

TEST_CASE(test_create_graph, {
    int nodes_amount = 20;

    graph_t *graph = create_graph(nodes_amount);

    CHECK_EQ_INT(graph->nodes, nodes_amount);
    for (int i = 0; i < nodes_amount; i++) {
        for (int j = 0; j < nodes_amount; j++) {
            CHECK_EQ_INT(graph->adj_matrix[i][j], 0);
        }
    }

    free(graph);
})

TEST_CASE(test_get_delivery_status, {
    CHECK_EQ_STRING(get_delivery_status(0), "NOT_DELIVERED");
    CHECK_EQ_STRING(get_delivery_status(1), "BEING_DELIVERED");
    CHECK_EQ_STRING(get_delivery_status(2), "DELIVERED");
    CHECK_FALSE(get_delivery_status(3) == "(null)");
})

//
// TEST resource_calculation.h
//

TEST_CASE(test_calculate_trucks, {
    package_t *package = create_package(1, 1, 1, 2, 2, 2, 5);
    double volume_filled = 0;
    double weight_filled = 0;
    int trucks_needed = 0;

    for (int i = 0; i < 20; i++) {
        calculate_trucks(*package, &volume_filled, &weight_filled, &trucks_needed);
    }
    free(package);

    CHECK_EQ_INT(trucks_needed, 9);
})

//
// TEST delivery_algorithm.h
//
TEST_CASE(test_a_star, {
    // Assert
    int nodes_amount = 4;
    graph_t *graph = create_graph(nodes_amount);

    for (int i = 0; i < nodes_amount; i++) {
        node_t *node = create_node(i+1, i+1, i+1);
        graph->node_addresses[i] = node;
    }

    add_edge(graph, 0, 1, ceil(heuristic(*graph->node_addresses[0], *graph->node_addresses[1])));
    add_edge(graph, 1, 2, ceil(heuristic(*graph->node_addresses[1], *graph->node_addresses[2])));
    add_edge(graph, 2, 3, ceil(heuristic(*graph->node_addresses[2], *graph->node_addresses[3])));

    graph_t optimized_matrix = *graph;



    a_star_matrix_t a_star_matrix = {
        create_graph(graph->nodes),
        &optimized_matrix
    };

    // Act
    for (int i = 0; i < graph->nodes - 1; i++) {
        for (int j = i + 1; j < graph->nodes; j++) {
            if(graph->adj_matrix[i][j] == 0) {
                a_star(graph, &a_star_matrix, graph->node_addresses[i], graph->node_addresses[j]);
            }
        }
    }

    int result_1 = a_star_matrix.optimized_matrix->adj_matrix[0][1];
    int result_2 = a_star_matrix.optimized_matrix->adj_matrix[0][2];
    int result_3 = a_star_matrix.optimized_matrix->adj_matrix[0][3];

    // Assert
    CHECK_EQ_INT(result_1, 2);
    CHECK_EQ_INT(result_2, 4);
    CHECK_EQ_INT(result_3, 5);

    free_matrix(graph);
    free(graph);
})


TEST_CASE(test_clarke_and_wright, {
    int nodes_amount = 4;

    graph_t *graph = create_graph(nodes_amount);

    for (int i = 0; i < nodes_amount; i++) {
        node_t *node = create_node(i+1, i+1, i+1);
        graph->node_addresses[i] = node;
    }

    add_edge(graph, 0, 1, ceil(heuristic(*graph->node_addresses[0], *graph->node_addresses[1])));
    add_edge(graph, 1, 2, ceil(heuristic(*graph->node_addresses[1], *graph->node_addresses[2])));
    add_edge(graph, 2, 3, ceil(heuristic(*graph->node_addresses[2], *graph->node_addresses[3])));

    graph_t optimized_matrix = *graph;

    a_star_matrix_t a_star_matrix = {
        create_graph(graph->nodes),
        &optimized_matrix
    };

    for (int i = 0; i < graph->nodes - 1; i++) {
        for (int j = i + 1; j < graph->nodes; j++) {
            if(graph->adj_matrix[i][j] == 0) {
                a_star(graph, &a_star_matrix, graph->node_addresses[i], graph->node_addresses[j]);
            }
        }
    }

    int depot = 0;

    int *routes = (int*)calloc(graph->nodes, sizeof(int));

    int **route_order = (int**)calloc(graph->nodes, sizeof(int*));
    for (int i = 0; i < graph->nodes; i++) {
        route_order[i] = (int*)calloc(graph->nodes, sizeof(int));
    }

    clarke_wright_algorithm(a_star_matrix, depot, routes, route_order);

    CHECK_EQ_INT(route_order[routes[1]][0], 1);
    CHECK_EQ_INT(route_order[routes[1]][1], 2);
    CHECK_EQ_INT(route_order[routes[1]][2], 3);

    free_matrix(graph);
    free(graph);
})

//
// TEST create_routes.h
//
TEST_CASE(test_add_edge, {
    int nodes_amount = 20;

    graph_t *graph = create_graph(nodes_amount);

    // Arrange
    int node_src = 1;
    int node_dst = 2;

    // Act
    add_edge(graph, node_src, node_dst, 1);
    int result_1 = graph->adj_matrix[node_src][node_dst];
    int result_2 = graph->adj_matrix[node_dst][node_src];

    // Assert
    CHECK_EQ_INT(result_1, 1);
    CHECK_EQ_INT(result_2, 1);

    // Arrange
    node_src = 3;
    node_dst = 4;

    // Act
    add_edge(graph, node_src, node_dst, 1);
    result_1 = graph->adj_matrix[node_src][node_dst];
    result_2 = graph->adj_matrix[node_dst][node_src];

    // Assert
    CHECK_EQ_INT(result_1, 1);
    CHECK_EQ_INT(result_2, 1);

    // Arrange
    node_src = 8;
    node_dst = 9;

    // Act
    add_edge(graph, node_src, node_dst, 1);
    result_1 = graph->adj_matrix[node_src][node_dst];
    result_2 = graph->adj_matrix[node_dst][node_src];

    // Assert
    CHECK_EQ_INT(result_1, 1);
    CHECK_EQ_INT(result_2, 1);

    free_matrix(graph);
    free(graph);
})

TEST_CASE(test_free_matrix, {
    int nodes_amount = 20;

    graph_t *graph = create_graph(nodes_amount);

    free_matrix(graph);

    CHECK_TRUE(graph->adj_matrix == NULL);
    free(graph);
})

//
// INCLUDE TESTS
//
MAIN_RUN_TESTS(
    test_generate_random_package,
    test_generate_random_node,
    test_generate_random_graph,
    test_create_truck,
    test_create_package,
    test_create_node,
    test_create_graph,
    test_get_delivery_status,
    test_calculate_trucks,
    test_a_star,
    test_clarke_and_wright,
    test_add_edge,
    test_free_matrix
);