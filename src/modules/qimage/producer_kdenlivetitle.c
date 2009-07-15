/*
 * producer_kdenlivetitle.c -- kdenlive producer
 * Copyright (c) 2009 Marco Gittler <g.marco@freenet.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <framework/mlt.h>
//#include "frei0r_helper.h"
#include <stdlib.h>
#include <string.h>
//#include <QtCore/QCoreApplication>
//#include <QtGui/QImage>
extern void init_qt();
extern uint8_t* refresh_kdenlivetitle(uint8_t*,int,int,double);

static int producer_get_image( mlt_frame frame, uint8_t **buffer, mlt_image_format *format, int *width, int *height, int writable )
{

    // Obtain properties of frame
    mlt_properties properties = MLT_FRAME_PROPERTIES( frame );

    // Obtain the producer for this frame
    mlt_producer producer = mlt_properties_get_data( properties, "producer_kdenlivetitle", NULL );

    // Obtain properties of producer
    mlt_properties producer_props = MLT_PRODUCER_PROPERTIES( producer );
    // Allocate the image
    int size = *width * ( *height  ) * 4;
    *buffer = mlt_pool_alloc( size);
    // Update the frame
    mlt_properties_set_int( properties, "width", *width );
    mlt_properties_set_int( properties, "height", *height );
    //printf("%x %x %x\n",*(buffer),*(buffer+1),*(buffer+2));
    
    //######### noise
    // Update the frame
    mlt_properties_set_data( properties, "image", *buffer, size, mlt_pool_release, NULL );
#if 0
    // Before we write to the image, make sure we have one
    if ( *buffer != NULL )
    {
        // Calculate the end of the buffer
        uint8_t *p = *buffer + *width * *height * 2;

        // Value to hold a random number
        uint32_t value;

        // Generate random noise
        while ( p != *buffer )
        {
            value = rand() & 0xff;
            *( -- p ) = 128;
            *( -- p ) = value < 16 ? 16 : value > 240 ? 240 : value;
        }
    }


    printf("%x %x\n", mlt_properties_get_data(properties,"image",NULL),mlt_properties_get_data(producer_props,"image",NULL));
    //unchached now
#else
    if ( 1 )
    {
        // Allocate the image
        *format = mlt_image_rgb24a;
        //*format = mlt_image_yuv422;
        mlt_position in = mlt_producer_get_in( producer );
        mlt_position out = mlt_producer_get_out( producer );
        mlt_position time = mlt_frame_get_position( frame );
        double position = ( double )( time - in ) / ( double )( out - in + 1 );
        //uint8_t *pic= mlt_pool_alloc( size);
        uint8_t* pic1=refresh_kdenlivetitle(*buffer, *width, *height, position );
        
        
        memcpy(*buffer,pic1,size); 
        
        /*uint8_t *p = *buffer + *width * *height * 2;

        // Value to hold a random number
        uint32_t value;

        // Generate random noise
        while ( p != *buffer )
        {
            value = rand() & 0xff;
            *( -- p ) = 128;
            *( -- p ) = value < 16 ? 16 : value > 240 ? 240 : value;
        }
        */
        mlt_log_debug( MLT_PRODUCER_SERVICE(producer), "width:%d height:%d %s\n",*width,*height, mlt_image_format_name( *format ) );
    }   
#endif
    return 0;
}

int producer_get_frame( mlt_producer producer, mlt_frame_ptr frame, int index )
{
    // Generate a frame
    *frame = mlt_frame_init( MLT_PRODUCER_SERVICE( producer ) );

    if ( *frame != NULL )
    {
        // Obtain properties of frame and producer
        mlt_properties properties = MLT_FRAME_PROPERTIES( *frame );

        // Obtain properties of producer
        mlt_properties producer_props = MLT_PRODUCER_PROPERTIES( producer );

        // Set the producer on the frame properties
        mlt_properties_set_data( properties, "producer_kdenlivetitle", producer, 0, NULL, NULL );

        // Update timecode on the frame we're creating
        mlt_frame_set_position( *frame, mlt_producer_position( producer ) );

        // Set producer-specific frame properties
        mlt_properties_set_int( properties, "progressive", 1 );
        mlt_properties_set_double( properties, "aspect_ratio", mlt_properties_get_double( producer_props, "aspect_ratio" ) );

        // Push the get_image method
        mlt_frame_push_get_image( *frame, producer_get_image );
    }

    // Calculate the next timecode
    mlt_producer_prepare_next( producer );

    return 0;
}

void producer_close( mlt_producer producer )
{
    producer->close = NULL;
    mlt_producer_close( producer );    

    free( producer );
}
mlt_producer producer_kdenlivetitle_init( mlt_profile profile, mlt_service_type type, const char *id, char *arg )
{
    // Create a new producer object
    mlt_producer this = mlt_producer_new( );

    // Initialise the producer
    if ( this != NULL )
    {
        init_qt(arg);
        // Callback registration
        this->get_frame = producer_get_frame;
        this->close = ( mlt_destructor )producer_close;
    }
    return this;
}

