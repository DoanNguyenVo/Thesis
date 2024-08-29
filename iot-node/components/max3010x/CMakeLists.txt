idf_component_register(SRCS "src/MAX30105.cpp" "src/heartRate.cpp" "src/spo2_algorithm.cpp"
                      INCLUDE_DIRS "src/."
                      REQUIRES arduino
                      )

project(max3010x)