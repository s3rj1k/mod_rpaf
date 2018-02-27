#include "ap_release.h"
#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_vhost.h"
#include "apr_strings.h"

#include <ctype.h> // isspace

module AP_MODULE_DECLARE_DATA ssl_stub_module;
APR_DECLARE_OPTIONAL_FN(int, ssl_is_https, (conn_rec *));

typedef struct {
    const char         *orig_scheme;
    const char         *https_scheme;
} ssl_stub_server_cfg;

static void *ssl_stub_create_server_cfg(apr_pool_t *p, server_rec *s) {
    ssl_stub_server_cfg *cfg = (ssl_stub_server_cfg *)apr_pcalloc(p, sizeof(ssl_stub_server_cfg));
    if (!cfg)
        return NULL;

    /* server_rec->server_scheme only available after 2.2.3 */
    #if AP_SERVER_MINORVERSION_NUMBER > 1 && AP_SERVER_PATCHLEVEL_NUMBER > 2
    cfg->orig_scheme = s->server_scheme;
    #endif

    cfg->https_scheme = apr_pstrdup(p, "https");

    return (void *)cfg;
}

static int ssl_stub_post_read_request(request_rec *r) {
    ssl_stub_server_cfg *cfg = (ssl_stub_server_cfg *)ap_get_module_config(r->server->module_config, 
                                                                   &ssl_stub_module);

    /* this overcomes an issue when mod_rewrite causes this to get called again
       and the environment value is lost for HTTPS. This is the only thing that
       is lost and we do not need to process any further after restoring the
       value. */
    const char *ssl_stub_https = apr_table_get(r->connection->notes, "ssl_stub_https");
    if (ssl_stub_https) {
        apr_table_set(r->subprocess_env, "HTTPS"      , ssl_stub_https);
        apr_table_set(r->subprocess_env, "SSL_TLS_SNI", r->hostname);
        return DECLINED;
    }

    const char *httpsvalue, *scheme;
    if ((httpsvalue = apr_table_get(r->headers_in, "X-Forwarded-Proto"))
               && (strcmp(httpsvalue, cfg->https_scheme) == 0)) {
        apr_table_set(r->connection->notes, "ssl_stub_https", "on");
        apr_table_set(r->subprocess_env   , "HTTPS"         , "on");
        apr_table_set(r->subprocess_env   , "SSL_TLS_SNI"   , r->hostname);
        scheme = cfg->https_scheme;
    } else {
        scheme = cfg->orig_scheme;
    }
    #if AP_SERVER_MINORVERSION_NUMBER > 1 && AP_SERVER_PATCHLEVEL_NUMBER > 2
    r->server->server_scheme = scheme;
    #endif

    return DECLINED;
}

static int ssl_is_https(conn_rec *c) {
    return apr_table_get(c->notes, "ssl_stub_https") != NULL;
}

static void ssl_stub_register_hooks(apr_pool_t *p) {
    ap_hook_post_read_request(ssl_stub_post_read_request, NULL, NULL, APR_HOOK_FIRST);

    /* this will only work if mod_ssl is not loaded */
    if (APR_RETRIEVE_OPTIONAL_FN(ssl_is_https) == NULL)
        APR_REGISTER_OPTIONAL_FN(ssl_is_https);
}

module AP_MODULE_DECLARE_DATA ssl_stub_module = {
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    ssl_stub_create_server_cfg,
    NULL,
    NULL,
    ssl_stub_register_hooks,
};
