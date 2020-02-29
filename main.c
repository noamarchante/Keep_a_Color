/*Realizar un programa que cree una imagen a partir de una imagen ya existente,
conservando un determinado color y sustituyendo el resto por su imagen en blanco
y negro. Como tecnica de paralelismo solo se pedirá empleo de threads*/

#include <stdio.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <emmintrin.h>
#include <pthread.h>

#define NTHREADS 4
#define ALTO  32
#define ANCHO 32

IplImage* ImgOriginal;
IplImage* ImgFinal;

void trabajo_thread(void*ptr) {
int*parametro = (int*) ptr;
 int fila, columna, i, j, fi, co;
    unsigned char Cb, Cg, Cr, Ca;
    float Bw;

    for (fila = *parametro*ALTO; fila < ImgOriginal->height; fila += ALTO*NTHREADS) {
        unsigned char *pImgOriginal, *pImgFinal;
        
        for (columna = 0; columna < ImgOriginal->width; columna += ANCHO) {

            for (i = 0; i < ALTO; i++) {
                for (j = 0; j < ANCHO; j++) {

                    pImgOriginal = (unsigned char *) ImgOriginal->imageData + ((fila + i) * ImgOriginal->widthStep)+((columna + j) * ImgOriginal->nChannels);
                    pImgFinal = (unsigned char *) ImgFinal->imageData + ((fila + i) * ImgFinal->widthStep)+((columna + j) * ImgFinal->nChannels);
                    Cb = *pImgOriginal++;
                    Cg = *pImgOriginal++;
                    Cr = *pImgOriginal++;

                    //COMPROBACION DE COLOR (ROJO)
                    if (Cr > 0x55 && Cb < 0x55 && Cg < 0x55) {
                        *pImgFinal++ = Cb;
                        *pImgFinal++ = Cg;
                        *pImgFinal++ = Cr;
                    } else {

                        //FACTORES DE CONVERSION A BLANCO Y NEGRO (Bw)
                        Bw = 0.114 * (Cb);
                        Bw += 0.587 * (Cg);
                        Bw += 0.299 * (Cr);

                        //ASIGNACION DE CONVERSION A BLANCO Y NEGRO
                        *pImgFinal++ = (unsigned char) Bw;
                        *pImgFinal++ = (unsigned char) Bw;
                        *pImgFinal++ = (unsigned char) Bw;
                    }

                    //COMPROBACION PARA QUE FUNCIONE CON IMAGENES DE 3 Y 4 CANALES (JPG Y PNG)
                    if (ImgOriginal->nChannels == 4) {
                        *pImgFinal++ = *pImgOriginal++;
                    }

                }
            
            //MUESTRA LA IMAGEN
            cvShowImage("Resultado", ImgFinal);
            
            //ACTIVA LA ESPERA
            cvWaitKey(10);
            }
           
        }
    }
}

int main(int argc, char** argv) {

    //COMPROBACIÓN DE QUE SE PASAN DOS VALORES COMO PARÁMETROS (NOMBRE DEL PROYECTO Y FOTO)
    if (argc != 2) {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    //CARGA LA IMAGEN PASADA AL PROYECTO
    ImgOriginal = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);

    //COMPRUEBA SI HA ENCONTRADO LA IMAGEN
    if (!ImgOriginal) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }
    

    //CONTENEDOR IMAGEN FINAL
    ImgFinal = cvCreateImage(cvSize(ImgOriginal->width, ImgOriginal->height), IPL_DEPTH_8U, ImgOriginal->nChannels);
       
    
    //CREACION DE LA VENTANA
    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
/*
    cvNamedWindow("Resultado", CV_WINDOW_NORMAL);
*/
    
    //MUESTRA LA IMAGEN
    cvShowImage(argv[1], ImgOriginal);
    
    pthread_t threads[NTHREADS];
    int filas[NTHREADS];
    int i;

    
    for (i = 0; i < NTHREADS; i++) {
        filas[i] = i;
        printf("\nEl thread %d sustituye las filas %d", i, filas[i]);
        pthread_create(&threads[i], NULL, (void*) &trabajo_thread, (void*) &filas[i]);
    }

    for (i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    //ACTIVA LA ESPERA
    cvWaitKey(0);
    
    //DESCARGA LA IMAGEN
    cvReleaseImage(&ImgOriginal);
    cvReleaseImage(&ImgFinal);

    // DESTRUYE LA VENTANA
    cvDestroyWindow(argv[1]);
    cvDestroyWindow("Resultado");

    return EXIT_SUCCESS;
}
