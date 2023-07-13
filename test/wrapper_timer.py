import time

def iteration_info_decorator(func):
    def wrapper(*args, **kwargs):
        start_time = time.time()
        total_iterations = len(args[0])  # Assuming the first argument is the iterable sequence

        for i, item in enumerate(args[0], 1):
            current_time = time.time()
            elapsed_time = current_time - start_time
            remaining_time = (elapsed_time / i) * (total_iterations - i)

            print(f"Iteration: {i}/{total_iterations}")
            print(f"Elapsed Time: {elapsed_time:.2f} seconds")
            print(f"Remaining Time: {remaining_time:.2f} seconds")
            print("")

            func(item, *args[1:], **kwargs)

    return wrapper

# Example usage
@iteration_info_decorator
def my_function(item):
    # Rest of the code inside the loop...
    print(f"Processing item: {item}")
    time.sleep(1)  # Simulating some processing

# Calling the decorated function
items = [1, 2, 3, 4, 5]
my_function(items)
