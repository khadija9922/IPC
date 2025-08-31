# Use a lightweight Linux image with build tools
FROM gcc:latest

# Set working directory inside container
WORKDIR /app

# Copy source code into container
COPY code.cpp .

# Compile C++ program
RUN g++ -O2 code.cpp -o code

# Default command when container runs
CMD ["./code"]
