    #pragma mark - replace with your own app id and app key
    #define APP_ID @"dd2ac4ef"
    #define APP_KEY @"d847841d5cc470ecedfc668933d3853c"

    NSString * language = @"en";
    NSString * word = @"Change";
    NSString * word_id = [word lowercaseString]; //word id is case sensitive and lowercase is required
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"https://od-api.oxforddictionaries.com:443/api/v1/inflections/%@/%@", language, word_id]];

    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url];
    [request setHTTPMethod:@"GET"];

    [request setValue:@"application/json" forHTTPHeaderField:@"Accept"];
    [request setValue:APP_ID forHTTPHeaderField:@"app_id"];
    [request setValue:APP_KEY forHTTPHeaderField:@"app_key"];

    NSURLSession *session = [NSURLSession sharedSession];
    NSURLSessionDataTask *task = [session dataTaskWithRequest:request
                                            completionHandler:
                                  ^(NSData *data, NSURLResponse *response, NSError *error) {

                                      if (error) {
                                          NSLog(@"%@",error);
                                          return;
                                      }

                                      if ([response isKindOfClass:[NSHTTPURLResponse class]]) {
                                          NSLog(@"Response HTTP Status code: %ld\n", (long)[(NSHTTPURLResponse *)response statusCode]);
                                          NSLog(@"Response HTTP Headers:\n%@\n", [(NSHTTPURLResponse *)response allHeaderFields]);
                                      }

                                      NSString* body = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
                                      NSLog(@"Response Body:\n%@\n", body);
                                  }];
    [task resume];