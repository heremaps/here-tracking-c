# Getting Started
HERE provides the HERE Tracking Client C Library to securely connect devices with [HERE Tracking](http://tracking.here.com). This library is primarily for embedded devices with limited resources and for porting to different embedded platforms. HERE also provides a HERE Tracking Client C++ Library. The choice is yours on which library you wish to use based on personal preference, target device requirements/performance or existing frameworks. 

The HERE Tracking Client C Library includes the following components:
- `libcheck` and `fff`-based in the `tests` directory
- `app` directory with a demo application including a sample implementation of class usage and library interfaces

## Requirements
To build the library, you need the following:
- C compiler supporting C89

The instructions below use `cmake` to build the application.

## Credentials
To use the sample application, or any application created with the HERE Tracking Client C Library, you need to:
1. Sign up for a developer account with HERE Tracking.
2. In the vendor role, create some devices with device licenses.
3. In the user role, claim those devices.

The device licenses include device IDs and device secrets.

## Building the Sample Application
To build the sample application on a Linux platform: 
1. Install the `libmbedtls-dev` development package.
2. In a `build` directory, run `cmake` with the following option: 
```
mkdir build
cd build
cmake -DBuildSampleApp=ON ..
make
```
`cmake` creates an output binary called: `here_tracking_app`.

 To use `here_tracking_app` from the command line, specify a device ID, a device secret and the HERE Tracking URL.

# Guide
The articles in this section provide a guide to using the HERE Tracking Client C Library. The guide describes common use cases for the API and illustrates them with practical examples.

## Porting
To port the library to a new platform, you need to implement the following interfaces:
- [Base64](@ref base64_if)
- [HMAC-SHA](@ref hmac_sha_if)
- [Log](@ref log_if) (optional, required only when logging is enabled)
- [Time](@ref time_if)
- [TLS](@ref tls_if)

## Using the Library
The example code below sends data to and receives data from HERE Tracking using the client interface.
```

here_tracking_error my_send_cb(uint8_t** data, size_t* data_size, void* user_data)
{
    /*
     * Set the data buffer to send and it's size:
     * *data = my_send_data;
     * *data_size = my_send_data_size;
     *
     * When no more data to send set:
     * *data = NULL;
     * *data_size = 0;
     */

    return HERE_TRACKING_OK;
}

here_tracking_error my_recv_cb(const here_tracking_recv_data* data, void* user_data)
{
    /* Handle received data here. */
    return HERE_TRACKING_OK;
}

int main(int argc, char** argv)
{
  here_tracking_client client;
  here_tracking_error err;
  static const char* thing_id = "my-thing-id";
  static const char* thing_secret = "my-thing-secret";
  static const char* base_url = "tracking.api.here.com";

  err = here_tracking_init(&client, thing_id, thing_secret, base_url);

  if(err == HERE_TRACKING_OK)
  {
      err = here_tracking_send_stream(&client,
                                      my_send_cb,
                                      my_recv_cb,
                                      HERE_TRACKING_RESP_WITH_DATA,
                                      NULL);
  }

  return (err == HERE_TRACKING_OK) ? 0 : -1;
}
```

## Building the Library
To build the library, perform the following steps:
1. To use `cmake`, create a build directory and run `cmake` as follows.
```
mkdir build
cd build
cmake ..
make
```
2. To build for Linux with Docker, run the provided build script.
```
./build.sh
```

## Tests

Unit tests written using [libcheck](https://libcheck.github.io/check/) and
[fff](https://github.com/meekrosoft/fff) are located in the *test* directory.

### Running Tests

To build and run the tests using CMake, execute:
```.sh
mkdir build
cd build
cmake -DBuildTests=ON ..
make && make test
```

To build and run the tests using Docker, execute:
```.sh
export TEST=1 && ./build.sh
```

## Logging
Log messages are disabled by default.

To enable log messages, set `HERE_TRACKING_LOG_LEVEL` to the desired level and rebuild the library.

## Service Support
If you need assistance with this or any other HERE product, contact your HERE representative.

## License
Copyright (c) 2017 HERE Europe B.V.

See the [LICENSE](./LICENSE) file in the root of this project for license details.