#ifndef _uFuzzy_h
#define _uFuzzy_h

/* term definition */
typedef struct{
    double value;
    double point1, point2, slope1, slope2;
} term;

/* linguistic variable definition */
typedef struct{
    term *terms;
    int number_of_terms;
} linguistic_variable;

/* input variables */
typedef struct{
    linguistic_variable *linguistic_variables;
    int number_of_variables;
} data_set;

/* single rule */
typedef struct{
    int *if_side;
    int *then_side;
} rule;

/* rule set */
typedef struct{
    rule *rules;
    int number_of_rules;
    int if_size;
    int then_size;
} rule_set;

data_set alloc_data_set(int number_of_variables);
linguistic_variable alloc_lv(int number_of_terms);
void dealloc_data_set(data_set *set);

rule_set alloc_rule_set(data_set *input, data_set *output, int number_of_rules);
void dealloc_rule_set(rule_set *rules);
void set_rule(rule_set *rules, int number_of_rule, ...);

/* create term with automatic calculation of slope */
term create_term(double a, double b, double c, double d);

/* base functions used for fuzzy inference */
void fuzzification(data_set *input, data_set *output, double input_values[]);
void rule_evaluation(data_set *input, data_set *output, rule_set *rules);
void defuzzification(data_set *output, double output_values[]);

/* main function - returns defuzzified result */
void fuzzy_calculate(data_set *input, data_set *output, rule_set *rules, double input_values[], double output_values[]);

#endif // _uFuzzy_h
