//
//  Alohar.h
//  Alohar
//
//  Created by Sam Warmuth on 2/23/12.
//  Last Updated by Jianming Zhou on 8/1/2012.
//  Copyright (c) 2011-2012 Alohar Mobile Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ALMotionState.h"
#import "ALResponse.h"
#import <CoreLocation/CoreLocation.h>
#import "ALPlace.h"
#import "ALUserStay.h"
#import "ALMobileState.h"

typedef enum {
    kPOWER_SAVE_MODE = 0,
    kFULL_MODE
} SERVICE_MODE;

/*!
 * Protocol for Registration and Authentication.
 */
@protocol ALSessionDelegate <NSObject>
@optional
/*!
 * Callback when user successfuly logs in.
 * \param userID The unique user ID in alohar system. The ID shall be kept safely for future use.
 */
- (void)aloharDidLogin:(NSString *)userID;
/*!
 * Callback when user failed to login. 
 * \param error Detailed error information.
 */
- (void)aloharDidFailWithError:(NSError *)error;
@end

/*!
 * Protocol to listen to motion state updates.
 */
@protocol ALMotionDelegate <NSObject>
@optional
/*!
 * Callback when motion state changes
 * \param newMotionState New motion state
 * \param oldMotionState Old motion state
 * @see ALMotionState
 */
- (void)didUpdateToMotionState:(ALMotionState *)newMotionState fromMotionState:(ALMotionState *)oldMotionState;

@end


/*!
 * Protocol to listen to high level mobile state updates.
 * \param newMobileState New mobile state
 * \param oldMobileState Old mobile state
 */
@protocol ALMobileStateDelegegate <NSObject>
@optional

/*!
 * Callback when mobile state changes
 * \param newMobileState New Mobile State
 * \param oldMobileState Old Mobile State
 */
- (void)didUpdateToMobileState:(ALMobileState *)newMobileState fromMobileState:(ALMobileState *)oldMobileState;

@end
/*!
 * Protocol to listen to real time events such as arrival, departure and user stay detection.
 */
@protocol ALUserStayDelegate <NSObject>
@optional
/*!
 * Callback when a user stay is detected or updated.
 * \param newStay The updated new user stay.
 */
- (void)currentUserStayIdentified:(ALUserStay *)newStay;

/*!
 * Callback when an arrival event is detected. 
 *
 * \param personalPlace The place you arrive. The SDK will only surface personal places, optionally.
 * \param location The centroid of the user's location when the arrival event was triggered.
 * 
 * @warning *important* The personal place will be set if and only if a)user has created a personal place b)the use's centroid is
 * close enough to that personal place's location.
 */
- (void)userArrivedAtPlace:(ALPlace *)personalPlace withLocation:(CLLocation *)location;

/*!
 * Callback when a departure event is detected. 
 * \param location The centroid of the last user stay. 
 * If no user stay is detected, the centroid of the user's last location is returned.
 */
- (void)userDepartedPlaceWithLocation:(CLLocation *)location;
@end

/*!
 */
enum {
    kALUserStayArrival,
    kALUserStayDeparture,
    kALUserStayUpdate
};

/*!
 * Alohar is the main interface to Alohar's framework. 
 *
 * To use the Alohar, the user needs to be authenticated. For
 * a new user, call ```registerWithAppID:andAPIKey:withDelegate```
 * or call ```authenticateWithAppID:andAPIKey:andUserID:withDelegate```
 * 
 * Once the user is authenticated, he/she can start to use rest of the
 * framework. Alohar is a persistent sensing framework, to enable 
 * persistent sensing, start the service by calling
 * ```
 * startMonitoringUser
 * ```
 * The service can be stopped any time by calling
 * ```
 * stopMonitoringUser
 * ```
 * 
 */
@interface Alohar : NSObject

@property (weak) id <ALSessionDelegate>  sessionDelegate;
@property (weak) id <ALMotionDelegate>   motionDelegate;
@property (weak) id <ALUserStayDelegate> userStayDelegate;
@property (weak) id <ALMobileStateDelegegate> mobileStateDelegate;
@property (nonatomic, strong) ALMotionState *currentMotionState;
@property (nonatomic, strong) ALMobileState *currentMobileState;


/** @name Initialization */

/*!
 * Get the Shared Instance of Alohar
 */
+ (Alohar *)sharedInstance;

/** @name Registration/Authentication */

/*!
 * Register a new user for the app using delegate response
 *
 * \param appID The AppID assigned for the App, go to https://www.alohar.com/developer to register your app.
 * \param APIKey The APIKey assigned for the App
 * \param delegate A delegate that comforms to ALSessionDelegate
 */
+ (void)registerWithAppID:(NSString *)appID andAPIKey:(NSString *)APIKey withDelegate:(id<ALSessionDelegate>)delegate;

/*!
 * Register a new user for the app using block response
 *
 * \param appID The AppID assigned for the App, click https://www.alohar.com/developer to register your app.
 * \param APIKey The APIKey assigned for the App
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 */
+ (void)registerWithAppID:(NSString *)appID andAPIKey:(NSString *)APIKey completion:(void (^)(NSString *userToken, NSError *error))completion;

/*!
 * Authenticate an existing user. 
 * For a new user, please use 
 * ```registerWithAppID:andAPIKey:withDelegate``` instead.
 * 
 * \param appID The AppID assigned for the App.
 * \param APIKey The APIKey assigned to the App.
 * \param userID The unique user ID assigned from Alohar from registration.
 * \param delegate A delegate that conforms to the ALSessionDelegate protocol.
 */
+ (void)authenticateWithAppID:(NSString *)appID andAPIKey:(NSString *)APIKey andUserID:(NSString *)userID withDelegate:(id<ALSessionDelegate>)delegate;

/*!
 * Authenticate an existing user using block. 
 * For new user, please use 
 * ```registerWithAppID:andAPIKey:completion``` instead.
 * 
 * \param appID The AppID assigned for the App.
 * \param APIKey The APIKey assigned to the App.
 * \param userID The unique user ID returned by Alohar at user registration.
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 */
+ (void)authenticateWithAppID:(NSString *)appID andAPIKey:(NSString *)APIKey andUserID:(NSString *)userID completion:(void (^)(NSString *userToken, NSError *error))completion;

/*!
 * Log out the current user from Alohar's cloud system.
 * 
 * It will reset all user's information and stop the monitoring service.
 * return YES if success.
 */
+ (BOOL)logout;

/** @name Life Cycle Of Monitering Service */

/*!
 * Start the monitoring service
 */
+ (void)startMonitoringUser;

/*!
 * Stop the monitoring service
 */
+ (void)stopMonitoringUser;

/*!
 * Change the service mode of the monitoring service
 * \param serviceMode The supporte service mode are: kPOWER_SAVE_MODE and kFULL_MODE.
 * 
 * @warning *important* kPOWER_SAVE_MODE is the default mode which has the best battery life performance which will intelligently stop the location service in some user cases. Switching between different mode will restart the service. 
 */
+ (void)switchServiceMode:(SERVICE_MODE)serviceMode;

/*!
 * Get current service mode.
 */
+ (NSInteger)currentServiceMode;

/*!
 * Load customized configuration for your application if any.
 * \param appID The AppID assigned for the App
 * \param APIKey The APIKey assigned to the App.
 * @warning *important* This method shall be called before any API calls.
 */
+ (void)customizeApp:(NSString *)appID withApiKey:(NSString *)APIKey;


/** @name Delegates */

/*!
 * Set Motion State Delegate
 * \param delegate A delegate that conforms to the ALMotionDelegate
 *
 */
+ (void)setMotionDelegate:(id <ALMotionDelegate>)delegate;

/*!
 * Set the User Stay Delegate 
 * \param delegate A delegate that conforms to the ALUserStayDelegate protocol
 *
 */
+ (void)setUserStayDelegate:(id <ALUserStayDelegate>)delegate;

/*!
 * Set the Mobile State Delegate
 * \param delegate A delegate that conforms to the ALMobileStateDelegate protocol
 *
 */
+ (void)setMobileStateDelegate:(id<ALMobileStateDelegegate>)delegate;

/** @name Place/User Stay Retrieval Methods */

/*!
 * Get the ALUserStay object for the user's current visit. If the user is between places, this will be nil.
 * @see ALUserStay
 */
+ (ALUserStay *)currentUserStay;

/*!
 * Get the user's user stays for a given date.
 * \param date The date to search for user stays. Counts from midnight to midnight of the user's time zone (e.g. July 8th is any user stay that overlaps with 12:01AM July 8th to 11:59PM July 8th).
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 * 
 * @warning *NOTE* The place candidate list of the user stay is suppressed by default to save bandwidth. You can use ```getPlaceCandidatesForStay:(NSInteger)stayId completion:(void (^)(ALResponse *response, NSError *error))completion``` to fetch the list.
 */
+ (void)getUserStaysForDate:(NSDate *)date completion:(void (^)(ALResponse *response, NSError *error))completion;

/*!
 * Get user stays that that start or end within a given time period
 * \param startDate The start time.
 * \param endDate The end time.
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 *
 * @warning *NOTE* The place candidate list of the user stay is suppressed by default to save bandwidth. You can use ```getPlaceCandidatesForStay:(NSInteger)stayId completion:(void (^)(ALResponse *response, NSError *error))completion``` to fetch the list.
 */
+ (void)getUserStaysFromDate:(NSDate *)startDate toDate:(NSDate *)endDate completion:(void (^)(ALResponse *response, NSError *error))completion;

/*!
 * Get user's user stays that start or end within a given time period and a location boundary.
 * 
 * \param startDate The start time
 * \param endDate The end time
 * \param location The centroid location of the search area. Optional.
 * \param radius The search radius in meter. Optinal. Skip if the location is not provided.
 * \param limit The limitation of total number matches to return. Optional. The default is 500.
 * \param includeCandidates Flag to indicate whether the user stay shall include its candidates. Optional. The default is NO.
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 *
 * @warning *NOTE* The place candidate list of the user stay is suppressed by default to save bandwidth. You can use ```getPlaceCandidatesForStay:(NSInteger)stayId completion:(void (^)(ALResponse *response, NSError *error))completion``` to fetch the list.
 * 
 */
+ (void)getUserStaysFromDate:(NSDate *)startDate toDate:(NSDate *)endDate atLocation:(CLLocation *)location radius:(NSInteger)radius limit:(NSInteger)limit includeCandidiates:(BOOL)includeCandidates completion:(void (^)(ALResponse *response, NSError *error))completion;


/*!
 * Get all places the current user has ever visited.
 * 
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 * 
 * @warning *NOTE* The response may be large (750KB+) depending on the total number of places user has visited. We recommend using ```getPlacesWithPattern:andCategory:withDelegate``` instead. 
 */
+ (void)getAllPlacesWithCompletion:(void (^)(ALResponse *response, NSError *error))completion;

/*!
 * Get places user visited with names that match the regular expression
 * 
 * \param namePattern The regular expression to match the place names against.
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 */

+ (void)getPlacesWithPattern:(NSString *)namePattern completion:(void (^)(ALResponse *response, NSError *error))completion;
/*!
 * Get places the user has visited that match the given name and category 
 * 
 * \param namePattern The regular expression to match the place names against.
 * \param catPattern The regular expression to match the place categories against.
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 */

+ (void)getPlacesWithPattern:(NSString *)namePattern andCategory:(NSString *)catPattern completion:(void (^)(ALResponse *response, NSError *error))completion;

/*!
 * Get places the user visited within a time window that match the category regex.
 * 
 * \param namePattern The regular expression to match the place names against.
 * \param startDate The start time
 * \param endDate The end Time
 * \param visits The minimum number of visits for each place. For example, if this is set to 3, all places with 1-2 visits will not be included in the results.
 * \param catPattern The regular expression to match the place's category
 * \param limit The limitation of total number matches to return. Pass nil to return the default number (500).
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 */
+ (void)getPlacesWithPattern:(NSString *)namePattern fromDate:(NSDate *)startDate toDate:(NSDate *)endDate minimalVisits:(NSInteger)visits withCategory:(NSString *)catPattern limit:(NSInteger)limit completion:(void (^)(ALResponse *response, NSError *error))completion;

/*!
 * Correct a user stay's selected POIs with a new place candidate
 *
 * \param stayId The user stay's stayId, required. Valid ID must be large than 0.
 * \param placeId The new selected place's ID, required. Valid placeId must be large than 0.
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 * 
 * If the params are not valid, NSError will be returned with empty response.
 *
 * This API is used to correct the wrong detection from Alohar from the place candidate list attached to the user stay.
 */
+ (void)correctStay:(NSInteger)stayId withCandidate:(NSInteger)placeId completion:(void (^)(ALResponse *response, NSError *error))completion;


/*!
 * Correct a user stay's selected POIs with a new place
 *
 * \param stayId The user stay's stayId (>0)
 * \param name The name of the new place, required.
 * \param loc The location of the new place, required.
 * \param addr The complete address of the new place, required
 * \param category The category type of the new place, required.
 * \param completion The complete handler block with ALResponse or Error.
 *
 * If the params are not valid, NSError will be returned with empty response.
 *
 * This API is used to correct the wrong detection from Alohar by creating a new personal place. Even if the place is public (not a personal POI), Alohar will treat as personal place in our system.
 * 
 */
+ (void)correctStay:(NSInteger)stayId withPlace:(NSString*)name location:(CLLocation*)loc address:(NSString*)addr category:(placeCategoryType)category completion:(void (^)(ALResponse *response, NSError *error))completion;

/*!
 * Delete a user stay
 * 
 * \param stayId The user stay's stayId. The valid stayId must be large than 0. Otherwise, NSException will be raised.
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 *
 * If the params are not valid, NSError will be returned with empty response.
 */
+ (void)deleteStay:(NSInteger)stayId completion:(void (^)(ALResponse *response, NSError *error))completion;

/*!
 * Get the place candidates for a user stay.
 * 
 * \param stayId The user stay's ID. @see ALUserStay. The valid stayId must be large than 0. Otherwise, NSException will be raised.
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 *
 * If the params are not valid, NSError will be returned with empty response.
 */
+ (void)getPlaceCandidatesForStay:(NSInteger)stayId completion:(void (^)(ALResponse *response, NSError *error))completion;

/*!
 * Get all user stays for a place.
 *
 * \param placeId A place's ID. @see ALPlace. The valid placeId must be large than 0. Otherwise, NSException will be raised.
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 *
 * If the params are not valid, NSError will be returned with empty response.
 */
+ (void)getStaysForPlace:(NSInteger)placeId completion:(void (^)(ALResponse *response, NSError *error))completion;

/*!
 * Get details for a place.
 * 
 * \param placeId Valid place's placeID. The valid placeId must be large than 0. Otherwise, NSException will be raised.
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 *
 * If the params are not valid, NSError will be returned with empty response.
 */
+ (void)getDetailsForPlace:(NSInteger)placeId completion:(void (^)(ALResponse *response, NSError *error))completion;

/*!
 * Get details for a user stay.
 *
 * \param stayId Valid user stay's stayID. The valid stayId must be large than 0. Otherwise, NSException will be raised.
 * \param completion This block is called on completion, with either a successful ALResponse or NSError describing the problem encountered.
 *
 * If the params are not valid, NSError will be returned with empty response.
 */
+ (void)getDetailsForStay:(NSInteger)stayId completion:(void (^)(ALResponse *response, NSError *error))completion;

/*!
 * Get all personal places you created.
 *
 * \param completion This block is called on completion, with either a success ALResponse or NSError describing the problem encountered.
 *
 */
+ (void)getPersonalPlacesWithCompletion:(void (^)(ALResponse *response, NSError *error))completion;

/** @name State Methods */

/*! 
 * Get the user's current location.
 *
 * @warning *important* the current location is the SDK's last known location of the device. 
 * If the monitoring service is stopped, the location will be stale. When the monitoring
 * service is running, it is close to the real current location in most cases. But it 
 * is NOT a replacement for the current location of native LocationManager. 
 */
+ (CLLocation *)currentLocation;

/*!
 * Get the device's current motion state.
 * @see ALMotionState
 */
+ (ALMotionState *)currentMotionState;

/*!
 * Get the device's current mobile state.
 * @see ALMobileState
 */
+ (ALMobileState *)currentMobileState;

/*!
 * Check whether the device is stationary.
 * \return YES if the device is NOT moving
 */
+ (BOOL)isStationary;

/*!
 * Check whether the user is in between user stays.
 * \return YES if the user has left their previous stay, but has not arrived at a new place.
 */
+ (BOOL)isBetweenUserStays;

/*!
 * Check whether the Alohar monitoring service is running.
 * \return YES if the monitoring service is ON
 */
+ (BOOL)monitoringUser;

/*!
 * Check whether the monitoring service is active.
 * 
 * \return YES if the location service is ON
 */
+ (BOOL) isLocationServiceOn;

/*!
 * Check whether the user has valid credentials and has an active session.
 * \return YES if the user connected to Alohar
 */
+ (BOOL)isLoggedIn;

/*!
 * Get Framework Version
 * \return The version of Alohar Framework
 */
+ (NSString *)version;

/*!
 * Get Register internal device ID
 *\return The internal device ID
 */
+ (NSString *)deviceId;

/*!
 * History of arrival/departure events.
 * \return Array of events
 * 
 * *Note*: There are three type of events: Arrival, Departure, UserStay
 * For Arrival/Departure events, the NSDictionary has the following key/value pairs:
 * ```{type:(NSString*), location:(CLLocation*), timestamp:(NSInteger)}```
 *
 * For Userstay events, the NSDictionary has the following key/value pairs:
 * ```{type:(NSString*), stay:(ALUserStay*), timestamp:(NSInteger)}```
 *
 * @warning *important* We will only keep at most 50 most recent events.
 */
+ (NSArray *)userStayLocationHistory;

/*!
 *
 * Save the in-memory data into disk.
 * It is recommended to trigger before the application is going to be terminated.
 */
+ (void)saveData;

/*!
 *
 * Upload remaining data to server right away. Currenly it does not suppoer onComplete/onError callback.
 * It is recommended to trigger flush data before logout or want to upload data right away.
 * Please note: this API is still in beta.
 */
//+ (void)flushData;

//PRIVATE

- (void)_manuallySetUserID:(NSString *)userID andToken:(NSString *)token;

- (void)motionStateChanged:(ALMotionState *)motionState;

- (void)userStayChanged:(ALUserStay *)userStay;

- (void)departPlace;

- (void)arrivePlace:(ALPlace *)place;

- (void)mobileStateChanged:(ALMobileState *)newMobileState;

- (void)downloadDeviceId;

+ (NSArray *)logs;

@end


