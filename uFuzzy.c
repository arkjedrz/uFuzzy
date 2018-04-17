#include "uFuzzy.h"
#include "stdlib.h"
#include "stdarg.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define UPPER_LIMIT 255

/*****************************
        SET ALLOCATION
*****************************/

data_set alloc_data_set(int number_of_variables){
    data_set set;
    set.number_of_variables = number_of_variables;
    set.linguistic_variables = calloc(number_of_variables, sizeof(linguistic_variable));
    return set;
}

linguistic_variable alloc_lv(int number_of_terms){
    linguistic_variable lv;
    lv.number_of_terms = number_of_terms;
    lv.terms = calloc(number_of_terms, sizeof(term));
    return lv;
}

void dealloc_data_set(data_set *set){
    linguistic_variable *curr_lv, *last_lv;
    curr_lv = set->linguistic_variables;
    last_lv = curr_lv + set->number_of_variables;

    while (curr_lv < last_lv){
        free(curr_lv->terms);
        ++curr_lv;
    }
    free(curr_lv);
}

/*****************************
        RULE ALLOCATION
*****************************/

rule_set alloc_rule_set(data_set *input, data_set *output, int number_of_rules){
    rule_set rules;
    rules.number_of_rules = number_of_rules;
    rules.if_size = input->number_of_variables;
    rules.then_size = output->number_of_variables;
    rules.rules = calloc(number_of_rules, sizeof(rule));

    rule *curr_rule = rules.rules;
    rule *last_rule = curr_rule + number_of_rules;
    while (curr_rule < last_rule){
        curr_rule->if_side = calloc(input->number_of_variables, sizeof(int));
        curr_rule->then_side = calloc(output->number_of_variables, sizeof(int));
        ++curr_rule;
    }
    return rules;
}

void dealloc_rule_set(rule_set *rules){
    rule *curr_rule = rules->rules;
    rule *last_rule = curr_rule + rules->number_of_rules;

    while (curr_rule < last_rule){
        free(curr_rule->if_side);
        free(curr_rule->then_side);
        ++curr_rule;
    }
    free(rules->rules);
}

void set_rule(rule_set *rules, int number_of_rule, ...){
    int i = 0;
    rule *selected_rule = rules->rules + number_of_rule;
    va_list args;
    va_start(args, number_of_rule);
    for (i = 0; i < rules->if_size; ++i)
        selected_rule->if_side[i] = va_arg(args, int);
    for (i = 0; i < rules->then_size; ++i)
        selected_rule->then_side[i] = va_arg(args, int);
    va_end(args);
}

/*****************************
        CALCULATION
*****************************/

static double compute_slope(double a, double b){
    return UPPER_LIMIT / (b-a);
}

term create_term(double a, double b, double c, double d){
    term str;
    str.point1 = a;
    str.point2 = d;
    str.slope1 = compute_slope(a,b);
    str.slope2 = compute_slope(c,d);
    str.value = 0;
    return str;
}

static void compute_degree_of_membership(double input, term *str){
    double degree;
    double delta1 = input - str->point1;
    double delta2 = str->point2 - input;
    if (delta1 <= 0 || delta2 <= 0)
        degree = 0;
    else{
        degree = MIN(str->slope1*delta1, str->slope2*delta2);
        degree = MIN(degree, UPPER_LIMIT);
    }
    str->value = degree;
}

void fuzzification(data_set *input, data_set *output, double input_values[]){
    linguistic_variable *curr_lv, *last_lv;
    term *curr_term, *last_term;
    double *curr_input = input_values;

    curr_lv = input->linguistic_variables;
    last_lv = curr_lv + input->number_of_variables;
    while (curr_lv < last_lv){
        curr_term = curr_lv->terms;
        last_term = curr_term + curr_lv->number_of_terms;
        while (curr_term < last_term){
            compute_degree_of_membership(*curr_input, curr_term);
            ++curr_term;
        }
        ++curr_input;
        ++curr_lv;
    }

    curr_lv = output->linguistic_variables;
    last_lv = curr_lv + output->number_of_variables;
    while (curr_lv < last_lv){
        curr_term = curr_lv->terms;
        last_term = curr_term + curr_lv->number_of_terms;
        while (curr_term < last_term){
            curr_term->value = 0;
            ++curr_term;
        }
        ++curr_lv;
    }
}

void rule_evaluation(data_set *input, data_set *output, rule_set *rules){
    int i = 0, j = 0;
    double strength = 0;
    rule *curr_rule = rules->rules;
    rule *last_rule = curr_rule + rules->number_of_rules;
    while (curr_rule < last_rule){
        for (i = 0; i < output->number_of_variables; ++i){
            strength = UPPER_LIMIT;
            for (j = 0; j < input->number_of_variables; ++j)
                strength = MIN(strength, input->linguistic_variables[j].terms[curr_rule->if_side[j]].value);

            int selected_output = curr_rule->then_side[i];
            output->linguistic_variables[i].terms[selected_output].value =
                MAX(strength, output->linguistic_variables[i].terms[selected_output].value);
        }
        ++curr_rule;
    }
}

static double compute_area_of_trapezoid(term *str){
    double base = str->point2 - str->point1;
    double run1 = str->value / str->slope1;
    double run2 = str->value / str->slope2;
    double top = base - run1 - run2;
    return str->value * (base + top) / 2.0;
}

void defuzzification(data_set *output, double output_values[]){
    double area, sum_of_areas, sum_of_products, centroid;
    linguistic_variable *curr_lv = output->linguistic_variables;
    linguistic_variable *last_lv = curr_lv + output->number_of_variables;
    double *curr_value = output_values;
    term *curr_term, *last_term;

    while (curr_lv < last_lv){
        sum_of_products = 0; sum_of_areas = 0;
        curr_term = curr_lv->terms;
        last_term = curr_term + curr_lv->number_of_terms;
        while (curr_term < last_term){
            area = compute_area_of_trapezoid(curr_term);
            centroid = (curr_term->point1 + curr_term->point2) / 2.0;
            sum_of_products += centroid * area;
            sum_of_areas += area;
            ++curr_term;
        }
        *curr_value = sum_of_areas != 0 ? sum_of_products / sum_of_areas : 0;
        ++curr_value;
        ++curr_lv;
    }
}

void fuzzy_calculate(data_set *input, data_set *output, rule_set *rules, double input_values[], double output_values[]){
    fuzzification(input, output, input_values);
    rule_evaluation(input, output, rules);
    defuzzification(output, output_values);
}
