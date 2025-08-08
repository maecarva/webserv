#ifndef HTTP_STATUS_CODES
#define HTTP_STATUS_CODES


/*
	1xx : Information
	2xx : Succès
	3xx : Redirection
	4xx : Erreur du client HTTP
	5xx : Erreur du serveur / du serveur d'application
*/

// 1xx
#define HTTP_CONTINUE                       		100
#define HTTP_SWITCH_PROTOCOL                		101
#define HTTP_PROCESSING                     		102
#define HTTP_EARLY_HINTS                    		103

// 2xx
#define HTTP_OK                             		200
#define HTTP_CREATED                        		201
#define HTTP_ACCEPTED                       		202
#define HTTP_NON_AUHORITATIVE_INFORMATION   		203
#define HTTP_NO_CONTENT                     		204
#define HTTP_RESET_CONTENT                  		205
#define HTTP_PARTIAL_CONTENT                		206
#define HTTP_MULTI_STATUS                   		207
#define HTTP_ALREADY_REPORTED               		208
#define HTTP_CONTENT_DIFFERENT              		210
#define HTTP_IM_USED                        		226

// 3xx
#define HTTP_MULTIPLE_CHOICES						300
#define HTTP_MOVED_PERMANENTLY						301
#define HTTP_FOUND									302
#define HTTP_SEE_OTHER								303
#define HTTP_NOT_MODIFIED							304
#define HTTP_USE_PROXY								305
#define HTTP_INUTILIZED								306
#define HTTP_TEMPORARY_REDIRECT						307
#define HTTP_PERMANENT_REDIRECT						308
#define HTTP_TOO_MANY_REDIRECTS						310

// 4xx
#define HTTP_BAD_REQUEST							400
#define HTTP_UNAUTHORIZED							401
#define HTTP_PAYMENT_REQUIRED						402
#define HTTP_FORBIDDEN								403
#define HTTP_NOT_FOUND								404
#define HTTP_METHOD_NOT_ALLOWED						405
#define HTTP_NOT_ACCEPTABLE							406
#define HTTP_PROXY_AUTH_REQUIRED					407
#define HTTP_REQUEST_TIME_OUT						408
#define HTTP_CONFLICT								409
#define HTTP_GONE									410
#define HTTP_LENGTH_REQUIRED						411
#define HTTP_PRECONDITION_FAILED					412
#define HTTP_REQUEST_ENTITY_TOO_LARGE				413
#define HTTP_REQUEST_URI_TOO_LONG					414
#define HTTP_UNSUPPORTED_MEDIA_TYPE					415
#define HTTP_REQUEST_RANGE_UNSATISFIABLE			416
#define HTTP_EXPECTATION_FAILED						417
#define HTTP_I_M_A_TEAPOT							418
#define HTTP_PAGE_EXPIRED							419
#define HTTP_BAD_MAPPING							421
#define HTTP_UNPROCESSABLE_ENTITY					422
#define HTTP_LOCKED									423
#define HTTP_METHOD_FAILURE							424
#define HTTP_TOO_EARLY								425
#define HTTP_UPGRADE_REQUIRED						426
#define HTTP_INVALID_DIGITAL_SIGNATURE				427
#define HTTP_PRECONDITION_REQUIRED					428
#define HTTP_TOO_MANY_REQUEST						429
#define HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE		431
#define HTTP_RETRY_WITH								449
#define HTTP_BLOCKED_BY_WINDOWS_PARENTAL_CONTROL	450
#define HTTP_UNAVAILABLE_FOR_LEGAL_REASON			451
#define HTTP_UNRECOVERABLE_ERROR					456


// 5xx
#define HTTP_INTERNAL_SERVER_ERROR					500
#define HTTP_NOT_IMPLEMENTED						501
#define HTTP_BAD_GATEWAY							502
#define HTTP_SERVICE_UNAVAILABLE					503
#define HTTP_GATEWAY_TIME_OUT						504
#define HTTP_HTTP_VERSION_NOT_SUPPORTED				505
#define HTTP_VARIANT_ALSO_NEGOTIATES				506
#define HTTP_INSIFFICIENT_STORAGE					507
#define HTTP_LOOP_DETECTED							508
#define HTTP_BANDWIDTH_LIMIT_EXCEEDED				509
#define HTTP_NOT_EXTENDED							510
#define HTTP_NETWORD_AUTH_REQUIRED					511

#endif