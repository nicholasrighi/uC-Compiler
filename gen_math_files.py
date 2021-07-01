#This generates .c files that only contain variable declerations and math assignements
import random

file_name = "math_test"

# Returns a set containing num_of_vars unique variable names
def generate_var_list(num_of_vars):
  var_list = []
  ascii_num = 97
  for i in range(num_of_vars):
    var_list.append(chr(ascii_num + i))

  return var_list 

# Generates a .c file containing a random number of variables. These variables 
# are then added together to form a result, which is then moded by 256 and then 
# that value is returned from main 
def gen_test_file(file_number, num_of_vars):
  with open("math_test_files/" + file_name + str(file_number) + ".c", "w") as math_file:

    math_file.write("int main(void) {\n")

    var_list = generate_var_list(num_of_vars)

    # generate variables 
    for var in var_list:
      math_file.write("\tint " + var + ";\n")
    math_file.write("\tint result;\n")
    math_file.write("\n")

    # assign variables
    for var in var_list:
      math_file.write("\t" + var + " = " + str(random.randrange(1, 10)) + ";\n")
    math_file.write("\tresult = ")

    op_list = [" + ", " - ", " * ", " / ", " & ", " | ", " < ", " > ", " >= ", " <= ", " && "]

    # perform random math operations on all variables
    for var in var_list[:-1]:
        math_file.write(var + random.choice(op_list))
    math_file.write(var_list[-1] + ";")
    math_file.write("\n")

    # return result to user
    math_file.write("\treturn result;\n")

    math_file.write("}\n")

if __name__ == "__main__":
  random.seed(123)
  for i in range(1000):
    gen_test_file(file_number = i, num_of_vars = 10)

