//
//  Alohar.h
//  Alohar
//
//  Created by Sam Warmuth on 2/23/12.
//  Copyright (c) 2012 Alohar Mobile Inc.. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ALMotionState.h"
#import "ALResponse.h"
#import <CoreLocation/CoreLocation.h>
#import "ALPlace.h"
#import "ALUserStay.h"

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
 * \param error Detail error information.
 */
- (void)aloharDidFailWithError:(NSError *)error;
@end

/*!
 * Common protocol for rest of Alohar requests.
 */
@protocol ALRequestDelegate <NSObject>
@optional
/*!
 * Callback when the request returns successfully. 
 * \param response An instance of ALResponse.
 */
- (void)aloharRequestFinished:(ALResponse *)response;
/*!
 *Callback when request fails with error.
 * \param error Detailed error infomation.
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
- (void)aloharDidFailWithError:(NSError *)error;
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
 * \param location The centroid of the user's location when the arrival event is triggered.
 */
- (void)userArrivedAtPlaceWithLocation:(CLLocation *)location;
/*!
 * Callback when a departure event is detected. 
 * \param location The centroid of the last user stay. 
 * If the user stay is not detected, the centroid of user's location will be returned.
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
 * To use the Alohar, user needs to authenticate first. For
 * a new user, call registerWithAppID:andAPIKey:withDelegate
 * or call authenticateWithAppID:andAPIKey:andUserID:withDelegate
 * 
 * Once the user is authenticated, he/she can start to use rest of
 * framework. Alohar is a persistent sensing framework, to enable the 
 * persistent sensing, user shall start the service by calling
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

@property (nonatomic, strong) ALMotionState *currentMotionState;


/*!
 * Get the Shared Instance of Alohar
 */
+ (Alohar *)sharedInstance;

/*!
 * Get Framework Version
 * \return The version of Alohar Framework
 */
+ (NSString *)version;

/*!
 * Register a new user for a given App
 *
 * \param appID The AppID assigned for the App, click https://www.alohar.com/developer to register your app.
 * \param APIKey The ApiKey assigned for the App
 * \param delegate A delegate comform to ALSessionDelegate
 */
+ (void)registerWithAppID:(NSString *)appID andAPIKey:(NSString *)APIKey withDelegate:(id<ALSessionDelegate>)delegate;


/*!
 * Authenticate an existing user. For new user, please use 
 * registerWithAppID:andAPIKey:withDelegate instead.
 * 
 * \param appID The AppID assigned for the App.
 * \param APIKey The ApiKey assigned to the App.
 * \param userID The unique user ID assigned from Alohar from registration.
 * \param delegate A delegate that conforms to the ALSessionDelegate protocol.
 */
+ (void)authenticateWithAppID:(NSString *)appID andAPIKey:(NSString *)APIKey andUserID:(NSString *)userID withDelegate:(id<ALSessionDelegate>)delegate;


+ (void)startMonitoringUser;
+ (void)stopMonitoringUser;

+ (void)setMotionDelegate:(id <ALMotionDelegate>)delegate;
+ (void)setUserStayDelegate:(id <ALUserStayDelegate>)delegate;

/*!
 * Get the current user stay object
 * @see ALUserStay
 */
+ (ALUserStay *)currentUserStay;

/*!
 * Get the user's user stays for a given date.
 * \param date The date to search user stay.
 * \param delegate A delegate that comforms to the ALRequestDelegate protocol.
 */
+ (void)getUserStaysForDate:(NSDate *)date withDelegate:(id <ALRequestDelegate>)delegate;

/*!
 * Get the user's user stays within a time period
 * \param startDate The start time.
 * \param endDate The end time.
 * \param delegate A delegate that comforms to the ALRequestDelegate protocol.
 */
+ (void)getUserStaysFromDate:(NSDate *)startDate toDate:(NSDate *)endDate withDelegate:(id <ALRequestDelegate>)delegate;
 
/*!
 * Get user's user stays within a time period and a location boundary.
 * 
 * \param startDate The start time
 * \param endDate The end time
 * \param location The centroid location of the search area. Optional.
 * \param radius The search radius in meter. Optinal. Skip if the location is not provided.
 * \param limit The limitation of total number matches to return. Optional. The default is 500.
 * \param includeCand Flag to indicate whether the user stay shall include its candidates. Optional. The default is NO.
 * \param delegate A delegate that comforms to the ALRequestDelegate protocol.
 * 
 */
+ (void)getUserStaysFromDate:(NSDate *)startDate toDate:(NSDate *)endDate atLocation:(CLLocation *)location radius:(NSInteger)radius limit:(NSInteger)limit includeCandidiates:(BOOL)includeCand withDelegate:(id<ALRequestDelegate>)delegate;

/*!
 * Get all places a user visited.
 * 
 * \param delegate A delegate that comforms to the ALRequestDelegate protocol.
 * 
 * *NOTE* The response might be large depends on the total number of the places user visisted. Recommend to use getPlaces:withCategory:withDelegate instead. 
 */
+ (void)getAllPlacesWithDelegate:(id <ALRequestDelegate>)delegate;

/*!
 * Get places user visited and match the given name
 * 
 * \param namePattern The regular expression to match the place name.
 * \param delegate A delegate that comforms to the ALRequestDelegate protocol.
 */

+ (void)getPlaces:(NSString *)namePattern withDelegate:(id<ALRequestDelegate>)delegate;
/*!
 * Get places user visited match the given name and category 
 * 
 * \param namePattern The regular expression to match the place name
 * \param catPattern The regular expression to match the place's category
 * \param delegate A delegate that comforms to the ALRequestDelegate protocol.
 */

+ (void)getPlaces:(NSString *)namePattern withCategory:(NSString *)catPattern withDelegate:(id<ALRequestDelegate>)delegate;

/*!
 * Get places the user visited within a time window that match the category regex.
 * 
 * \param namePattern The regular expression for the place name
 * \param startDate The start time
 * \param endDate The end Time
 * \param visits The mininal numer of visits required for that places. Optioanl. The default is 1.
 * \param catPattern The regular expression to match the place's category
 * \param limit The limitation of total number matches to return. Optional. The default is 500.
 * \param delegate A delegate that comforms to the ALRequestDelegate protocol.
 */
+ (void)getPlaces:(NSString *)namePattern fromDate:(NSDate *)startDate toDate:(NSDate *)endDate minimalVisits:(NSInteger)visits withCategory:(NSString *)catPattern limit:(NSInteger)limit withDelegate:(id<ALRequestDelegate>)delegate;

/*!
 * Get the place candidates of a user stay.
 * 
 * \param stay A user stay. @see ALUserStay
 * \param delegate A delegate that comforms to the ALRequestDelegate protocol.
 */
+ (void)getPlaceCandidatesForStay:(ALUserStay *)stay withDelegate:(id<ALRequestDelegate>)delegate;

/*!
 * Get all user stays of a place.
 *
 * \param place A place. @see ALPlace
 * \param delegate A delegate that comforms to the ALRequestDelegate protocol.
 */
+ (void)getStaysForPlace:(ALPlace *)place withDelegate:(id<ALRequestDelegate>)delegate;

/*!
 * Get details for a place.
 * 
 * \param place Valid place. 
 * \param delegate A delegate that comforms to the ALRequestDelegate protocol.
 */
+ (void)getDetailsForPlace:(ALPlace *)place withDelegate:(id<ALRequestDelegate>)delegate;

/*!
 * Get details for a user stay.
 *
 * \param stay Valid user stay.
 * \param delegate A delegate that comforms to the ALRequestDelegate protocol.
 */
+ (void)getDetailsForStay:(ALUserStay *)stay withDelegate:(id<ALRequestDelegate>)delegate;

/*! 
 * Get the user's current location.
 */
+ (CLLocation *)currentLocation;

/*!
 * Get the device's current motion state.
 * @see ALMotionState
 */
+ (ALMotionState *)currentMotionState;

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
 \return YES if the monitoring service is ON
 */
+ (BOOL)monitoringUser;

/*!
 * Check whether the user has valid credentials and has an active session.
 * \return YES if the user connected to Alohar
 */
+ (BOOL)isLoggedIn;

/*!
 * History of arrival/departure events.
 * \return Array of events
 * 
 * Note: There are three type of events: Arrival, Departure, UserStay
 * For Arrival/Departure events, the NSDictionary has the following key/value pairs:
 * {type:(NSString*), location:(CLLocation*), timestamp:(NSInteger)}
 *
 * For Userstay events, the NSDictionary has the following key/value pairs:
 * {type:(NSString*), stay:(ALUserStay*), timestamp:(NSInteger)}
 *
 */
+ (NSArray *)userStayLocationHistory;

//PRIVATE

- (void)_manuallySetUserID:(NSString *)userID andToken:(NSString *)token;

- (void)motionStateChanged:(ALMotionState *)motionState;

- (void)userStayChanged:(ALUserStay *)userStay;

- (void)departLocation;

- (void)arriveLocation;


//PRIVATE
+ (NSArray *)logs;

@end


