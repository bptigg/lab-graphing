#pragma once

#include <vector>
#include <string>

struct features
{
	std::string title;
	std::string x_label;
	std::string y_lable;
};

struct graph_data
{
	std::vector<float> x;
	std::vector<float> y;

	std::vector<float> x_u;
	std::vector<float> y_u;
};

struct settings
{
	bool error_bars;
	bool title;
	bool axis_lables;
	
	bool linear_regression;
	bool deming_regression;

	bool exponetial_fit;
	bool polynomial_fit;
};

struct graph
{
	graph_data data;
	features graph_features;
	settings graph_settings;
};