#include <time.h>

#include "mtest.h"
#include "delivery_algorithm.h"
#include "delivery_sim.h"
#include "resource_calculation.h"

TEST_CASE(test_generate_random_package, {
    srand(time(NULL));

    for (int i = 0; i < 5; i++) {
        package_t package = generate_random_package();

        CHECK_EQ_INT(package.height >= 1 && package.height <= 2, 1);
        CHECK_EQ_INT(package.width >= 1 && package.width <= 2, 1);
        CHECK_EQ_INT(package.length >= 1 && package.length <= 2, 1);
        CHECK_EQ_INT(package.weight >= 1 && package.weight <= 25, 1);
        CHECK_EQ_INT(package.priority >= 1 && package.priority <= 5, 1);
        CHECK_EQ_INT(package.node_id >= 1 && package.node_id <= 5, 1);
        CHECK_EQ_INT(package.truck_id >= 1 && package.truck_id <= 5, 1);
    }
})

TEST_CASE(test_create_truck, {
    int truck_id = 1;

    truck_t truck = create_truck(truck_id);

    CHECK_EQ_INT(truck.id == truck_id, 1);
    CHECK_EQ_INT(truck.max_weight == 3500, 1);
    CHECK_EQ_INT(truck.average_speed == 60, 1);
    CHECK_EQ_INT(truck.max_volume == 19, 1);
})

TEST_CASE(test_create_package, {
    int package_priority = 1;
    int node_id = 2;
    int truck_id = 3;
    int height = 4;
    int width = 5;
    int length = 6;
    int weight = 7;

    package_t package = create_package(package_priority, node_id, truck_id, height, width, length, weight);

    CHECK_TRUE(package.priority == package_priority);
    CHECK_TRUE(package.node_id == node_id);
    CHECK_TRUE(package.truck_id == truck_id);
    CHECK_TRUE(package.height == height);
    CHECK_TRUE(package.width == width);
    CHECK_TRUE(package.length == length);
    CHECK_TRUE(package.weight == weight);
})

TEST_CASE(test_create_node, {
    int area = 1;
    int location_x = 2;
    int location_y = 3;
    int id = 4;

    node_t node = create_node(area, location_x, location_y, id);

    CHECK_TRUE(node.area == area);
    CHECK_TRUE(node.location_x == location_x);
    CHECK_TRUE(node.location_y == location_y);
    CHECK_TRUE(node.id == id);

    // Check if packages are initialized
    for (int i = 0; i < 30; i++) {
        CHECK_TRUE(node.packages[i].priority == -1);
        CHECK_TRUE(node.packages[i].node_id == -1);
        CHECK_TRUE(node.packages[i].truck_id == -1);
        CHECK_TRUE(node.packages[i].height == 0);
        CHECK_TRUE(node.packages[i].width == 0);
        CHECK_TRUE(node.packages[i].length == 0);
        CHECK_TRUE(node.packages[i].weight == 0);
    }
})

MAIN_RUN_TESTS(test_generate_random_package, test_create_truck, test_create_package, test_create_node);