/**
 * @file svg_test.h
 *
 */

 #ifndef SVG_TEST_H
 #define SVG_TEST_H
 
 #include "uikit/uikit.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 // stability test for svg drawing
 void uikit_draw_test_tiger(char* info[], int size, void* param);
 
 // function test for svg drawing
 void uikit_draw_demo_tigers(char* info[], int size, void* param);

 #ifdef __cplusplus
 } /* extern "C" */
 #endif
 
 #endif /*SVG_TEST_H*/
 