#include <stdio.h>
#include "uFuzzy.h"

/* enums are used to name linguistic variables and terms -- not required */
enum {LV_FIRST, LV_SECOND} input_lv_names;
enum {TERM_LOW, TERM_HIGH} term_names;

/* data sets for input and output and a rule set */
data_set input;
data_set output;
rule_set rules;

/* initialization function */
void init_fuzzy(){
    /* allocate data set with two linguistic variables */
    input = alloc_data_set(2);
    /* allocate linguistic variable with two terms */
    input.linguistic_variables[LV_FIRST] = alloc_lv(2);
    /* create triangle-shaped term named "TERM_LOW" in linguistic variable "LV_FIRST" */
    input.linguistic_variables[LV_FIRST].terms[TERM_LOW] = create_term(-20,-5,-5,10);
    /* another one */
    input.linguistic_variables[LV_FIRST].terms[TERM_HIGH] = create_term(-10,5,5,20);
    input.linguistic_variables[LV_SECOND] = alloc_lv(2);
    input.linguistic_variables[LV_SECOND].terms[TERM_LOW] = create_term(-20,-5,-5,10);
    input.linguistic_variables[LV_SECOND].terms[TERM_HIGH] = create_term(-10,5,5,20);

    /* define outputs */
    /* remember that rules have to affect all output
    - use separated data-sets and rule-sets when outputs have to be independent */
    output = alloc_data_set(2);
    output.linguistic_variables[LV_FIRST] = alloc_lv(2);
    output.linguistic_variables[LV_FIRST].terms[TERM_LOW] = create_term(-20,-5,-5,10);
    output.linguistic_variables[LV_FIRST].terms[TERM_HIGH] = create_term(-10,5,5,20);
    output.linguistic_variables[LV_SECOND] = alloc_lv(2);
    output.linguistic_variables[LV_SECOND].terms[TERM_LOW] = create_term(-20,-5,-5,10);
    output.linguistic_variables[LV_SECOND].terms[TERM_HIGH] = create_term(-10,5,5,20);

    /* allocate rule set with 2 rules */
    rules = alloc_rule_set(&input, &output, 2);
    /* set first rule */
    /* set_rule function uses variadic
     - first, declare "if" side of rule (", TERM_HIGH, TERM_HIGH, ")
     - then, declare "then" side of rule ("TERM_HIGH, TERM_LOW)");
     length of sides have to correspond with number of inputs/outputs */
    set_rule(&rules, 0, TERM_HIGH, TERM_HIGH, TERM_HIGH, TERM_LOW);
    set_rule(&rules, 1, TERM_LOW, TERM_LOW, TERM_LOW, TERM_HIGH);
}

int main()
{
    init_fuzzy();
    /* input values array */
    double inputValues[] = {5, 5};
    /* empty output values array */
    double outputValues[] = {0, 0};
    /* calculate values */
    fuzzy_calculate(&input, &output, &rules, inputValues, outputValues);
    /* display results */
    printf("Result: %f %f\n", outputValues[0], outputValues[1]);
    /* clean-up */
    dealloc_rule_set(&rules);
    dealloc_data_set(&input);
    dealloc_data_set(&output);
    return 0;
}
