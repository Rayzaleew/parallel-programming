// Задача 36.Любое число n, большее единицы, порождает последовательность вида:
// ni+1 = (ni % 2 == 0? ni / 2 : 3 * ni + 1),
// последним элементом которой является 1 (например: 13->40->20->10->5->16->8->4->2->1). 
// Вычислить количество последовательностей заданной длины n среди чисел заданного интервала от N до M.
#include <CL/cl.h>
#include <iostream>
#include <omp.h>
#include <vector>
#include <fstream>
#define MAX_SOURCE_SIZE (0x100000)

using namespace std;

vector<int> SerialSequence(int n)
{
    //int len = 1;
    vector<int> result;
    while (n > 1)
    {
        n = n % 2 == 0 ? n / 2 : 3 * n + 1;
        result.push_back(n);
        //len++;
    }
    return result;
}

int SerialFind(int n, int N, int M, ofstream &out)
{
    int counter = 0;
    vector<int> result;
    //int len = 0;
    for (int i = N; i <= M; i++)
    {
        result = SerialSequence(i);
        if (result.size()== n)
        {
            out << "Последовательность #" << counter+1 << ": "; 
            for (auto i : result)
            {
                out << i << " ";
            }
            out << endl;
            counter++;
        }
    }

    return counter;
}

int ParallelSequence(int n, int N, int M)
{
    int i;
    int size = 0;
    //for (int i = 0; )
    const int l_size = M-N + (64 - (M-N) % 64);
    //const int l_size = M-N;

    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("kernel.cl", "r");
    if (!fp)
    {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }

    source_str = (char *)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    //cout << ret << endl;
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id,
                         &ret_num_devices);
    //cout << ret << endl;

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    //cout << context << endl;
    cl_command_queue command_queue =
        clCreateCommandQueue(context, device_id, 0, &ret);
    //cout << command_queue << endl;
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                      l_size * sizeof(int), NULL, &ret);
    //cout << c_mem_obj << endl;
    cl_program program =
        clCreateProgramWithSource(context, 1, (const char **)&source_str,
                                  (const size_t *)&source_size, &ret);
    //cout << program << endl;
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    //cout << ret << endl;
    cl_kernel kernel = clCreateKernel(program, "findSequenceGpu", &ret);
    //cout << kernel << endl;
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &c_mem_obj);
    //cout << ret << endl;
    ret = clSetKernelArg(kernel, 1, sizeof(int), &N);
    //cout << ret << endl;

    size_t global_item_size = l_size;
    size_t local_item_size = 64;
    ret =
        clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size,
                               &local_item_size, 0, NULL, NULL);
    //cout << ret << endl;
    int *arr = (int *)malloc(sizeof(int) * l_size);
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,
                              l_size * sizeof(int), arr, 0, NULL, NULL);
    //cout << ret << endl;
    //for (i = 0; i < l_size; i++) printf("%d\n", arr[i]);
    // int max = arr[0];
    // int max_n = 1;
    int counter = 0;
    for (int i = 0; i <= M-N; i++)
        if (arr[i]==n)
        {
            counter++;
        }
    //cout << arr[0] << endl;
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(arr);
    return counter;
}

int main()
{
    int n, N, M;
    ofstream out;
    out.open("output.txt");
    double start, end;
    double start1, end1;
    int result = 0;
    cout << "Введите n: ";
    cin >> n;
    cout << "Введите N: ";
    cin >> N;
    cout << "Введите M: ";
    cin >> M;
    start = omp_get_wtime();
    result = SerialFind(n, N, M, out);
    cout << "Число последовательностей (serial): " << result << endl;
    end = omp_get_wtime();
    cout << "Время выполнения: " << end - start << " секунд" << endl;

    start1 = omp_get_wtime();
    result = ParallelSequence(n, N, M);
    cout << "Число последовательностей (parallel): " << result << endl;
    end1 = omp_get_wtime();
    cout << "Время выполнения: " << end1 - start1 << " секунд" << endl;

}
