/**
* ��������� ����.
*/

#include "StdAfx.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <CL/opencl.h>
#include <oclUtils.h>
#include <shrUtils.h>

#define LOW_PRECISION_PHYSICAL_CONSTANT
#include "PhysicalConstant.hpp"

#include "struct.h"

namespace phc = physical::constant;
namespace phcc = physical::constant::correlation;


// �������������� ��������� ���� ������ ����� OpenCL/image2d.
// @see (!) http://developer.apple.com/library/mac/#documentation/Performance/Conceptual/OpenCL_MacProgGuide/ImageProcessingwithOpenCL/ImageProcessingwithOpenCL.html#//apple_ref/doc/uid/TP40008312-CH103-SW2
// @see http://www.cmsoft.com.br/index.php?option=com_content&view=category&layout=blog&id=115&Itemid=172
// @see http://www.khronos.org/message_boards/viewtopic.php?f=28&t=2358
// ��� ������ � ������������
//   > http://www.imagemagick.org/Magick++/Image.html
//   > http://labs.qt.nokia.com/2010/04/07/using-opencl-with-qt


// �������� ���������� � �������
#define DEBUG_VERBOSE 0


// ���� � ������ OpenCL
const string PATH_EXECUTE = "D:/Projects/workspace/phygen/resource";
const string PATH_OPENCL = "D:/Projects/workspace/phygen/resource/OpenCL";

// ��������� ��� OpenCL
const string PLATFORM_OPENCL = "NVIDIA";


// ����� ���� ������������� � ������. ���� ����� ���� ���� - �� ����������
// ������������.

// ������� ����� � ����, �����
size_t pulse = 0;

// ����� ����� ��������� ���� � ����. ���������
const unsigned long TICK_PAUSE = 0 * 1000;
// ��������������� ���������� ��� ��������� �����
unsigned long  tickPause = 0;


// ������ ����
const size_t N = 1024;
const size_t M = 1024 / 2;
const size_t NM = N * M;

// �� ���� �������� ���� ����� ����������� ���������� ����
// (��������, ���� �������)
const cl_float DEFAULT_FORCE_X = 0.0f;
//const cl_float DEFAULT_FORCE_Z = -9.81f;
const cl_float DEFAULT_FORCE_Z = 0.0f;


// ��� �������������
RenderWhat viewMode = IMAGE_INTERACTION;

// ��������� �����
const size_t GRID_WORK_DIM = 2;
const size_t GRID_GLOBAL_WORK_SIZE[] = { N, M };
const size_t GRID_LWS = 16;
const size_t GRID_LOCAL_WORK_SIZE[] = { GRID_LWS, GRID_LWS / 2 };


cl_device_id* devices = nullptr;
cl_context context = nullptr;
cl_command_queue commandQueue = nullptr;
cl_kernel kernel = nullptr;
cl_program program = nullptr;


// ���������� ��� ���� GLUT
int glutWindowHandle = 0;

// ����� �������� ��� OpenGL
GLuint pbo = 0;
cl_mem pbo_cl = nullptr;


// �������: ����� � ���� �������
bool pause = false;




// ������ ��� ������ � �����
void createWorld();
void interaction();
void createImage();
void snapshotWorld();
void cellInfo( size_t a, size_t b );
void clickAction( size_t a, size_t b );


// OpenCL
void render();

// OpenGL
void initGL( int* argc, char** argv );
void displayGL();
void mouseGL( int button, int state, int x, int y );
void reshape( int w, int h );
void idle();
void keyboardGL( unsigned char key, int x, int y );
void initGLBuffers();
void deleteVBO( GLuint* vbo, cl_mem vbo_cl );

void cleanup( int exitCode );



// �������
// @see createWorld()
Particle* particle = nullptr;


// ��� N x M
WParticle* wp = nullptr;
cl_mem oclWP = nullptr;





int main( int argc, char** argv ) {

	setlocale( LC_ALL, "Russian" );
    // ��� ����������� '.' ������ ','
    setlocale( LC_NUMERIC, "C" );


    initGL( &argc, argv );


    cl_int status = CL_SUCCESS;

    // �������� ���������
    cl_platform_id platform;
    //status = u::ocl::getPlatformID( &platform, PLATFORM_OPENCL );
    status = oclGetPlatformID( &platform );
    assert( status == CL_SUCCESS );

    // �������� ����������
    cl_uint numDevices;
    status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices );
    assert( status == CL_SUCCESS );
    //cl_device_id* devices = (cl_device_id *)malloc(numDevices * sizeof(cl_device_id) );
    devices = new cl_device_id[ numDevices ];
    assert( devices );
    status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, numDevices, devices, nullptr );
    assert( status == CL_SUCCESS );

    //cout << getDeviceInfo( device );

    // ���� ������ �� ��������� ������ � �������� ����������
    cl_uint deviceUsed = 0;
    unsigned int endDevice = numDevices - 1;
    if( shrGetCmdLineArgumentu( argc, (const char**)argv, "device", &deviceUsed ) ) {
        deviceUsed = CLAMP( deviceUsed, 0, endDevice );
        endDevice = deviceUsed;
    } 

    // �������� ����� ������ ����������
    //cl_device_id device = u::ocl::getMaxFlopsDevice( context );
    cl_device_id device = devices[deviceUsed];

    // ������ ��������
    /* - ��������. ��. ����.
    context = clCreateContext(
        0, numDevices, devices, nullptr, nullptr, &status
    );
    */
    cl_context_properties props[] = {
        CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0
    };
    context = clCreateContext( props, 1, &devices[deviceUsed], nullptr, nullptr, &status );
    assert( status == CL_SUCCESS );

    oclPrintDevInfo( LOGBOTH, devices[deviceUsed] );

    // ������ ������� ������
    commandQueue = clCreateCommandQueue( context, device, 0, &status );
    assert( status == CL_SUCCESS );

    // �������� ����� ���������
    /* - ��������. ��. ����.
    const string kernelName = "render";
    const string sourcePath = PATH_OPENCL + "/" + kernelName + ".cl";
    size_t program_length;
    char* sourceCL = u::ocl::loadProgramSource(
        sourcePath.c_str(), "", &program_length
    );
    assert( sourceCL );
    */
    const string kernelName = "render";
    size_t programLength;
    //auto pathAndName = shrFindFilePath( ( kernelName + ".cl").c_str(), PATH_EXECUTE.c_str() );
    //assert( pathAndName );
    const string pathAndName = PATH_OPENCL + "/" + kernelName + ".cl";

    // (!)
    // ����������� ��������� ���������� (���?). � �.�. ������ ������ �� ���������
    // hcl-������, ��������� � ����� �� ��� ����� �� ������ �� ��� �������.
    // ������ �������� ����������� ������ ��� ������ ���������.
    // @todo fine ���������� �� ����� �������.
    ostringstream header;
    header << "// " << GetTickCount();
    auto sourceCL = oclLoadProgSource(
        pathAndName.c_str(),
        header.str().c_str(),
        &programLength
    );
    assert( sourceCL );
    
    // ������ ���������
    program = clCreateProgramWithSource(
        context,
        1,
		(const char **) &sourceCL,
        &programLength,
        &status
    );
    assert( status == CL_SUCCESS );
    
    // ����������� ���������
    ostringstream ss;
    ss << " -D N=" << N
       << " -D M=" << M
       << " -D NM=" << NM
       /* - ������������ � struct.hcl
       << " -D COUNT_ALL_PARTICLE=" << COUNT_ALL_PARTICLE
       */
       << " -D DEFAULT_FORCE_X=" << DEFAULT_FORCE_X
       << " -D DEFAULT_FORCE_Z=" << DEFAULT_FORCE_Z
       << "";
    string option = " \
        -I " + PATH_OPENCL + " \
        -cl-opt-disable \
        -Werror \
    " + ss.str();
    // @todo optimize ��������� > "-cl-fast-relaxed-math", "-cl-mad-enable", "-cl-opt-disable" and other
    // @see OpenCL spec. 5.4.3, http://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clBuildProgram.html
    status = clBuildProgram(
        program, 0, nullptr,
        option.c_str(),
        nullptr, nullptr
    );
    if (status != CL_SUCCESS) {
        shrLogEx( LOGBOTH | ERRORMSG, status, STDERROR );
        oclLogBuildInfo( program, oclGetFirstDev( context ) );
        //oclLogPtx( program, oclGetFirstDev( context ), "phygen.ptx" );
        oclLogPtx( program, oclGetFirstDev( context ), nullptr );
        cleanup( EXIT_FAILURE ); 
    }
    assert( status == CL_SUCCESS );

    // ������ ����������� ����.
    // �������: ���� ��������� ������ ���������� �� ����� �����.
    kernel = clCreateKernel( program, kernelName.c_str(), &status );
    assert( status == CL_SUCCESS );


    // ������ ������ ����������� � ��������� �� � GLUT
    initGLBuffers();

    // ��������� ���
    createWorld();

    // ������ ����� ����. ������������ ������� �������������.
    interaction();
    createImage();

    // ������ ����
    snapshotWorld();

    // ��������� �������� ���� ����������
    glutMainLoop();


    // ���������� ������: ������� �� �����
    cleanup( EXIT_SUCCESS );

    return 0;
}








/**
* �������������� GL.
*/
void initGL( int* argc, char **argv ) {
    // init GLUT 
    glutInit( argc, (char**)argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_ALPHA | GLUT_SINGLE );
    glutInitWindowSize( N, M );
    glutInitWindowPosition(
        glutGet( GLUT_SCREEN_WIDTH ) / 2 - N / 2, 
        glutGet( GLUT_SCREEN_HEIGHT ) / 2 - M / 2
    );
    glutWindowHandle = glutCreateWindow( "Interaction" );

    // OpenGL properties
    // Background color
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    // @see other in reshape()

    // register GLUT callback functions
    glutDisplayFunc( displayGL );
    glutMouseFunc( mouseGL );
    glutKeyboardFunc( keyboardGL );
    glutReshapeFunc( reshape );
    glutIdleFunc( idle );

    // init GLEW
    glewInit();
    GLboolean bGLEW = glewIsSupported(
        "GL_VERSION_2_0 GL_ARB_pixel_buffer_object"
    );
    assert( bGLEW );

}







/**
* ������ ������ ��� ������ � ������������.
*/
void initGLBuffers() {

    cout << "initGLBuffers()" << endl;

    // ���������� ������ ������ �����������.
    // 1 ������� = 4 ������: RGBA, float
    const size_t V_IMAGE = 4 * sizeof( cl_float ) * NM;

    //@todo optimize? �������� �������� ��� ���������� �����
    //      �������� image2d_t > http://www.songho.ca/opengl/gl_pbo.html#unpack

    // ������ Pixel Buffer Object (PBO)
    // @see http://www.songho.ca/opengl/gl_pbo.html
    // @see http://www.songho.ca/opengl/gl_vbo.html#create
    glGenBuffersARB(
        // ���������� �������
        1,
        // ������������� �������
        &pbo
    );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, pbo );
    glBufferDataARB(
        // GLenum target
        GL_PIXEL_UNPACK_BUFFER_ARB,
        // GLsizei size, � ������
        V_IMAGE,
        // const void* data - ������ ��� �����������
        nullptr,
        // GLenum usage
        // @todo optimize? http://www.songho.ca/opengl/gl_vbo.html
        GL_STREAM_DRAW_ARB
    );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
    
    /* test
    auto imageOut = new cl_float4[NM];
    for (size_t z = 0; z < M; z++) {
        for (size_t x = 0; x < N; x++) {
            const size_t i = x + z * N;
            imageOut[i].s[0] = (float)z / (float)M;
            imageOut[i].s[1] = (float)x / (float)M;
            imageOut[i].s[2] = (float)(z + x) / (float)(M + N);
            imageOut[i].s[3] = 1.0f;
        }
    }
    */

    // � ����� �� ����������� ����� � ������ OpenCL
    cl_int status;
    pbo_cl = clCreateBuffer(
        context,
        CL_MEM_WRITE_ONLY,
        //CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
        V_IMAGE,
        nullptr,
        //imageOut,
        &status
    );
    assert( status == CL_SUCCESS );

    status = clSetKernelArg( kernel, 0, sizeof( cl_mem ), (void*)&pbo_cl );
    assert( status == CL_SUCCESS );


    // ������ ��� �������� ������������� ������ ��������� � createWorld()

}






/**
* ���������� ������. ������� �� �����.
*/
void cleanup( int exitCode ) {

    clReleaseMemObject( pbo_cl );
    if ( pbo ) {
        glBindBuffer( 1, pbo );
        glDeleteBuffers( 1, &pbo );
    }

    delete[] wp;
    delete[] particle;

    delete[] devices;
    clReleaseKernel( kernel );
    clReleaseProgram( program );
    clReleaseCommandQueue( commandQueue );
    clReleaseContext( context );

    getchar();

    exit ( exitCode );
}






/**
* ������ �������� ����.
*/
void createWorld() {

    cout << "createWorld()" << endl;

    cout << "������ �������: " << N << " x " << M << " = " << NM << " �����" << endl;

    // �������.
    // ������ � ���� ������, ����� ��������� ���������� �������
    // ��� ��������� ����. ������ - ���������� (����� �������� ��� � OpenCL).
    // ������� ������ �����. ������������� UIDParticle.
    particle = new Particle[ COUNT_ALL_PARTICLE ];
    cout << "������ ��� Particle: " << sizeof( Particle ) * COUNT_ALL_PARTICLE << " ����" << endl;

    // ��������� �������������� ������

    UIDParticle uid = VACUUM_PARTICLE;
    particle[uid].color = 0x00000000;

    uid = BASALT_PARTICLE;
    particle[uid].color = 0x464451ff;

    uid = CLAY_PARTICLE;
    particle[uid].color = 0xe49b0fff;

    uid = SOGGY_CLAY_PARTICLE;
    particle[uid].color = 0xe28b00aa;

    uid = WATER_PARTICLE;
    particle[uid].color = 0x42aaff80;


    // ��������� ���.
    // ��� ����� �������� N x M: 1 ������ = 1 �������.

    // �������� ��� ���� �������.
    // ���������� ������ � ���� ������� �� ��������� N x M.
    wp = new WParticle[ NM ];
    cout << "������ ��� ����� ���� WParticle: " <<
        sizeof( WParticle ) * NM / 1024 / 1024 << " ��" <<
    endl;


    // ������� ������� ����
    for (size_t z = 0; z < M; ++z) {
        for (size_t x = 0; x < N; ++x) {
            const size_t i = x + z * N;
            wp[i].particle = VACUUM_PARTICLE;
            wp[i].mass = 0.0f;
            wp[i].velocityX = wp[i].velocityZ = 0.0f;
            wp[i].direction = 0;
            wp[i].temperature = 0.0f;  // � �������� ��������
        }
    }


    // ������ ���� ��������
    const size_t CLAY_BLOCK_WIDTH = 400;
    const size_t CLAY_BLOCK_HEIGHT = 200;
    const size_t SHIFT_X = N / 4 + 10;
    const size_t SHIFT_Z = M / 4 + 10;
    uid = CLAY_PARTICLE;
    for (size_t z = SHIFT_Z; z < SHIFT_Z + CLAY_BLOCK_HEIGHT; z++) {
        for (size_t x = SHIFT_X; x < SHIFT_X + CLAY_BLOCK_WIDTH; x++) {
            // ���������� ������ � ������� ����
            const size_t i = x + z * N;
            wp[i].particle = uid;
        }
    }


    // ��������� ���������� � �������� � ������� ���� � ������ OpenCL.
    // ����������� ����� ��� �����������.

    cl_int status = CL_SUCCESS;
    
    cl_mem oclParticle = clCreateBuffer(
        context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof( Particle ) * COUNT_ALL_PARTICLE,
        particle,
        &status
    );
    assert( status == CL_SUCCESS );


    oclWP = clCreateBuffer(
        context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
        sizeof( WParticle ) * NM,
        wp,
        &status
    );
    assert( status == CL_SUCCESS );


    cout << "������ ��� ��������� ����� WParticle: " <<
        sizeof( WParticle ) * NM / 1024 / 1024 << " ��" <<
    endl;
    cl_mem oclWPT = clCreateBuffer(
        context, CL_MEM_READ_WRITE,
        sizeof( WParticle ) * NM,
        nullptr,
        &status
    );
    assert( status == CL_SUCCESS );


    cout << "������ ��� ��������� ����� WParticleInteraction: " <<
        sizeof( WParticleInteraction ) * NM / 1024 / 1024 << " ��" <<
    endl;
    cl_mem oclWPI = clCreateBuffer(
        context, CL_MEM_READ_WRITE,
        sizeof( WParticleInteraction ) * NM,
        nullptr,
        &status
    );
    assert( status == CL_SUCCESS );


    // ��� ������ � ������ ���������� OpenCL
    status |= clSetKernelArg( kernel, 1, sizeof(cl_mem), (void*)&oclParticle );
    status |= clSetKernelArg( kernel, 2, sizeof(cl_mem), (void*)&oclWP );
    // ... (��������������� ����� ������� kernel)
    // @see createImage()
    status |= clSetKernelArg( kernel, 5, sizeof(cl_mem), (void*)&oclWPT );
    status |= clSetKernelArg( kernel, 6, sizeof(cl_mem), (void*)&oclWPI );
    assert( status == CL_SUCCESS );

}





/**
* ����� ����.
* �������� ���� ��������� ������ � ���� ���������� OpenCL.
*/
void createImage() {

    //cout << "createImage() " << endl;

    cl_int status = CL_SUCCESS;

    const RenderWhat what = PREPARE_INTERACTION;
    status |= clSetKernelArg( kernel, 3, sizeof( RenderWhat ), &what );
    const cl_uint randomNumber = 0;
    status |= clSetKernelArg( kernel, 4, sizeof( cl_uint ), &randomNumber );
    assert(status == CL_SUCCESS);

    // @see http://www.songho.ca/opengl/gl_pbo.html
    status = clEnqueueNDRangeKernel(
        commandQueue,
        kernel,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE, GRID_LOCAL_WORK_SIZE,
        0, 0, 0
    );
    status |= clFinish( commandQueue );
    assert( status == CL_SUCCESS );


    // ������ � ������ ���������� (����� �����������)
    // 1 ������� = 4 ������: RGBA, float
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, pbo );    
    // ����� ������ ������� - � ������ �������
    GLfloat* ptr = (GLfloat*)glMapBufferARB(
        GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB
    );
    status = clEnqueueReadBuffer(
        commandQueue,
        pbo_cl,
        CL_TRUE,
        0,
        4 * sizeof( cl_float ) * NM,
        ptr,
        0, nullptr, nullptr
    );
    assert( status == CL_SUCCESS );

    glUnmapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB );
}







/**
* ������ ��������������.
* �������������� ����� OpenCL.
*/
void interaction() {

    //cout << "interaction() " << endl;

    // ������������ ��������� ���� �� ���������� �����
    // ����� �� ���������, �������� ����� ������

    RenderWhat what = PREPARE_INTERACTION;
    cl_int status = clSetKernelArg( kernel, 3, sizeof( cl_uint ), &what );
    const cl_uint randomNumber = rand();
    status |= clSetKernelArg( kernel, 4, sizeof( cl_uint ), &randomNumber );
    assert(status == CL_SUCCESS);
    status = clEnqueueNDRangeKernel(
        commandQueue,
        kernel,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE, GRID_LOCAL_WORK_SIZE,
        0, 0, 0
    );
    assert( status == CL_SUCCESS );
    status = clFinish( commandQueue );
    assert( status == CL_SUCCESS );


    what = SELF_INTERACTION;
    status = clSetKernelArg( kernel, 3, sizeof( cl_uint ), &what );
    assert(status == CL_SUCCESS);
    status = clEnqueueNDRangeKernel(
        commandQueue,
        kernel,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE, GRID_LOCAL_WORK_SIZE,
        0, 0, 0
    );
    assert( status == CL_SUCCESS );
    status = clFinish( commandQueue );
    assert( status == CL_SUCCESS );


    what = NEIGHBOUR_INTERACTION;
    status = clSetKernelArg( kernel, 3, sizeof( cl_uint ), &what );
    assert(status == CL_SUCCESS);
    status = clEnqueueNDRangeKernel(
        commandQueue,
        kernel,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE, GRID_LOCAL_WORK_SIZE,
        0, 0, 0
    );
    assert( status == CL_SUCCESS );
    status = clFinish( commandQueue );
    assert( status == CL_SUCCESS );


    what = TEMP_COMMIT_INTERACTION;
    status = clSetKernelArg( kernel, 3, sizeof( cl_uint ), &what );
    assert(status == CL_SUCCESS);
    status = clEnqueueNDRangeKernel(
        commandQueue,
        kernel,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE, GRID_LOCAL_WORK_SIZE,
        0, 0, 0
    );
    assert( status == CL_SUCCESS );
    status = clFinish( commandQueue );
    assert( status == CL_SUCCESS );


    what = COMMIT_INTERACTION;
    status = clSetKernelArg( kernel, 3, sizeof( cl_uint ), &what );
    assert(status == CL_SUCCESS);
    status = clEnqueueNDRangeKernel(
        commandQueue,
        kernel,
        GRID_WORK_DIM,
        nullptr,
        GRID_GLOBAL_WORK_SIZE, GRID_LOCAL_WORK_SIZE,
        0, 0, 0
    );
    assert( status == CL_SUCCESS );
    status = clFinish( commandQueue );
    assert( status == CL_SUCCESS );

}





/**
* ��������� WParticle (GPU -> CPU).
*/
void updateWMapToCPU() {
    cl_int status = clEnqueueReadBuffer(
        commandQueue,
        oclWP,
        CL_TRUE,
        0,
        sizeof( WParticle ) * NM,
        wp,
        0, nullptr, nullptr
    );
    assert( status == CL_SUCCESS );
}




/**
* ��������� WParticle (CPU -> GPU).
*/
void updateWMapToGPU() {
    cl_int status = clEnqueueWriteBuffer(
        commandQueue,
        oclWP,
        CL_TRUE,
        0,
        sizeof( WParticle ) * NM,
        wp,
        0, nullptr, nullptr
    );
    assert( status == CL_SUCCESS );
}






/**
* ������ ����.
*/
void snapshotWorld() {
    // ��������� �� GPU ����� �������
    updateWMapToCPU();

    size_t countP[COUNT_ALL_PARTICLE];
    countP[VACUUM_PARTICLE] =
        countP[BASALT_PARTICLE] =
        countP[CLAY_PARTICLE] =
        countP[SOGGY_CLAY_PARTICLE] =
        countP[WATER_PARTICLE] =
    0;
    for (size_t iz = 0; iz < M; ++iz) {
        for (size_t ix = 0; ix < N; ++ix) {
            const size_t i = ix + iz * N;
            const auto pp = wp[i].particle;
            countP[ pp ]++;
        }
    }

    cout << dec;
    cout << "\t������ \t����� \t���� \t������ \t����" << endl;
    cout << "count \t" <<
        countP[VACUUM_PARTICLE] << " \t" << 
        countP[BASALT_PARTICLE] << " \t" << 
        countP[CLAY_PARTICLE] << " \t" << 
        countP[SOGGY_CLAY_PARTICLE] << " \t" << 
        countP[WATER_PARTICLE] <<
    endl;
    cout << endl;
}



/**
* ���������� � ������.
*/
void cellInfo( size_t x, size_t z ) {
    // ��������� �� GPU ����� �������
    updateWMapToCPU();

    if ( (x > N) || (z > M) ) {
        cout << dec;
        cout << "������ " << x << " " << z << " ����� �� ��������� ����" << endl;
        return;
    }

    const size_t i = x + z * N;
    cout << dec;
    cout << "coord " << x << " " << z << endl;
    cout << "<particle>" << endl;

    // Particle
    const auto p = wp[i];

    cout << "matter " << p.particle << endl;

    cout << "mass " << p.mass << endl;
    cout << "velocity " << p.velocityX << " " << p.velocityZ << endl;
    cout << "direction " << p.direction << endl;
    cout << "temperature " << p.temperature << endl;

    cout << endl;
}




/**
* �������� ��� ������� �������� �������� �������������� 'clickRightMode'.
*/
void clickAction( size_t x, size_t z ) {
    /* - �� ����������.
    // ��������� �� GPU ����� �������
    updateWMapToCPU();

    
    // ClickWhat::HOT_1000
    const size_t i = x + z * N;
    wm[i].pe[0].temperature = 1000.0f;


    // ���������� ��������� � GPU
    updateWMapToGPU();

    // ��������� ��������
    //interaction( 0.0f );
    createImage();
    glutPostRedisplay();

    // ������ ������ ����
    snapshotWorld();
    */
}









/**
* ������������ ��� � ������� OpenGL.
*/
void render() {
    
#if 1
    // �� ���������� ����� ��� ����
    const unsigned long tickCurrent = GetTickCount();
    if ( (tickCurrent - tickPause) < TICK_PAUSE ) {
        return;
    }
    tickPause = tickCurrent;
#endif

    // ������������ ��������������
    interaction();

    pulse++;
    cout << "pulse > " << pulse << endl;

    // ��������� ����� ����� ����
    createImage();
    glutPostRedisplay();
}






void displayGL() {

#if 0
    // test
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
        glVertex2f(0.25, 0.25);
        glVertex2f(0.75, 0.25);
        glVertex2f(0.75, 0.75);
        glVertex2f(0.25, 0.01 * time);
    glEnd();
    glFlush();
    time += 1.1;

/*
    // @source http://www.gamedev.net/community/forums/topic.asp?topic_id=489283
	//glClearColor( 1, 0, 0, 0 );
	glClear( GL_COLOR_BUFFER_BIT );
    auto glutError = glGetError();
    assert(glutError == 0);

    struct RGBA {
	    float r;
	    float g;
	    float b;
	    float a;
    };
	RGBA* pixels = new RGBA[ NM ];
	for (unsigned int i = 0; i < NM/2 - 100; i++) {
		pixels[i].r = 0.0;
		pixels[i].g = 0.5;
		pixels[i].b = 0.5;
		pixels[i].a = 0.1f;
	}
	
	glDrawPixels(N, M, GL_RGBA, GL_FLOAT, pixels);
    glutError = glGetError();
    assert(glutError == 0);

    glFlush();
	//glutSwapBuffers();

    //delete[] pixels;
*/
#endif

    
    // ���������� ���������
    glClear( GL_COLOR_BUFFER_BIT );
    glDisable( GL_DEPTH_TEST );
    glRasterPos2i( 0, 0 );

    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, pbo );
    auto glutError = glGetError();
    assert(glutError == 0);

    glDrawPixels( N, M, GL_RGBA, GL_FLOAT, nullptr );
    glutError = glGetError();
    assert(glutError == 0);

    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
    glutError = glGetError();
    assert(glutError == 0);

    // ����������� ����� �� �����
    // @todo ������������ ������������� � ���� ��������.
    glutSwapBuffers();
    glutError = glGetError();
    assert(glutError == 0);

    //glScaled(2.0, 2.0, 2.0);

    glFlush();

    //glutReportErrors();


    // ���-�� ����� �������� � ���������
    ostringstream ss;
    ss << "viewMode: " << viewMode;
    glutSetWindowTitle( ss.str().c_str() );

}





// @see http://www.lighthouse3d.com/opengl/glut/index.php?9
void mouseGL(int button, int state, int x, int y) {
    if (state != GLUT_DOWN) {
        return;
    }

    const int zz = M - y - 1;

    if (button == GLUT_LEFT_BUTTON) {
        snapshotWorld();
        cellInfo( x, zz );
        return;
    }
    
    if (button == GLUT_RIGHT_BUTTON) {
        snapshotWorld();
        cellInfo( x, zz );
        clickAction( x, zz );
        return;
    }
    
}





void idle() {
    if ( !pause ) {
        // �������
        render();
    }
}





// Window resize handler callback
//*****************************************************************************
void reshape( int x, int y ) {
    glViewport( 0, 0, x, y );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, 1.0, 0.0, 1.0, 0.0, 1.0 ); 
}





// Keyboard event handler callback
//*****************************************************************************
void keyboardGL(unsigned char key, int x, int y) {
    switch ( key ) {
        case '1':
            viewMode = IMAGE_INTERACTION;
            break;

        /*
        case '2':
            viewMode =
                (viewMode == IMAGE_WMAP)
                  ? IMAGE_MATTER_NEIGHBORS_DENSITY
                  : IMAGE_WMAP;
            break;
        */

        /* - �� ����������.
        case 'h':
            clickRightMode = HOT_1000;
            cout << "clickRightMode: HOT 1000" << endl;
            break;
        */

        case '=':
        case '+':
            interaction();
            break;

        case ' ':
            pause = !pause;
            break;

        // �����
        case '\033':
        case '\015':
        case 'Q':
        case 'q':
            exit( EXIT_SUCCESS );
            break;
    }

    // ��������� ������� � ���������� ��
    createImage();
    glutPostRedisplay();

}
