/*
 * LibSylph -- E-Mail client library
 * Copyright (C) 1999-2006 Hiroyuki Yamamoto
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "defs.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "prefs.h"
#include "prefs_account.h"
#include "customheader.h"
#include "account.h"
#include "utils.h"

static PrefsAccount tmp_ac_prefs;

static PrefParam param[] = {
	/* Basic */
	{"account_name", NULL, &tmp_ac_prefs.account_name, P_STRING},
	{"is_default", "FALSE", &tmp_ac_prefs.is_default, P_BOOL},
	{"name", NULL, &tmp_ac_prefs.name, P_STRING},
	{"address", NULL, &tmp_ac_prefs.address, P_STRING},
	{"organization", NULL, &tmp_ac_prefs.organization, P_STRING},
	{"protocol", NULL, &tmp_ac_prefs.protocol, P_ENUM},
	{"receive_server", NULL, &tmp_ac_prefs.recv_server, P_STRING},
	{"smtp_server", NULL, &tmp_ac_prefs.smtp_server, P_STRING},
	{"nntp_server", NULL, &tmp_ac_prefs.nntp_server, P_STRING},
	{"use_nntp_auth", "FALSE", &tmp_ac_prefs.use_nntp_auth, P_BOOL},
	{"user_id", "ENV_USER", &tmp_ac_prefs.userid, P_STRING},
	{"password", NULL, &tmp_ac_prefs.passwd, P_STRING},
	{"inbox", "inbox", &tmp_ac_prefs.inbox, P_STRING},

	/* Receive */
	{"use_apop_auth", "FALSE", &tmp_ac_prefs.use_apop_auth, P_BOOL},
	{"remove_mail", "TRUE", &tmp_ac_prefs.rmmail, P_BOOL},
	{"message_leave_time", "0", &tmp_ac_prefs.msg_leave_time, P_INT},
	{"get_all_mail", "FALSE", &tmp_ac_prefs.getall, P_BOOL},
	{"enable_size_limit", "FALSE", &tmp_ac_prefs.enable_size_limit, P_BOOL},
	{"size_limit", "1024", &tmp_ac_prefs.size_limit, P_INT},
	{"filter_on_receive", "TRUE", &tmp_ac_prefs.filter_on_recv, P_BOOL},
	{"imap_check_inbox_only", "FALSE", &tmp_ac_prefs.imap_check_inbox_only,
	 P_BOOL},
	{"imap_auth_method", "0", &tmp_ac_prefs.imap_auth_type, P_ENUM},
	{"max_nntp_articles", "300", &tmp_ac_prefs.max_nntp_articles, P_INT},
	{"receive_at_get_all", "TRUE", &tmp_ac_prefs.recv_at_getall, P_BOOL},

	/* Send */
	{"add_date", "TRUE", &tmp_ac_prefs.add_date, P_BOOL},
	{"generate_msgid", "TRUE", &tmp_ac_prefs.gen_msgid, P_BOOL},
	{"add_custom_header", "FALSE", &tmp_ac_prefs.add_customhdr, P_BOOL},
	{"use_smtp_auth", "FALSE", &tmp_ac_prefs.use_smtp_auth, P_BOOL},
	{"smtp_auth_method", "0", &tmp_ac_prefs.smtp_auth_type, P_ENUM},
	{"smtp_user_id", NULL, &tmp_ac_prefs.smtp_userid, P_STRING},
	{"smtp_password", NULL, &tmp_ac_prefs.smtp_passwd, P_STRING},
	{"pop_before_smtp", "FALSE", &tmp_ac_prefs.pop_before_smtp, P_BOOL},

	/* Compose */
	{"signature_type", "0", &tmp_ac_prefs.sig_type, P_ENUM},
	{"signature_path", "~" G_DIR_SEPARATOR_S DEFAULT_SIGNATURE,
	 &tmp_ac_prefs.sig_path, P_STRING},
	{"set_autocc", "FALSE", &tmp_ac_prefs.set_autocc, P_BOOL},
	{"auto_cc", NULL, &tmp_ac_prefs.auto_cc, P_STRING},
	{"set_autobcc", "FALSE", &tmp_ac_prefs.set_autobcc, P_BOOL},
	{"auto_bcc", NULL, &tmp_ac_prefs.auto_bcc, P_STRING},
	{"set_autoreplyto", "FALSE", &tmp_ac_prefs.set_autoreplyto, P_BOOL},
	{"auto_replyto", NULL, &tmp_ac_prefs.auto_replyto, P_STRING},

	/* Privacy */
	{"default_sign", "FALSE", &tmp_ac_prefs.default_sign, P_BOOL},
	{"default_encrypt", "FALSE", &tmp_ac_prefs.default_encrypt, P_BOOL},
	{"encrypt_reply", "TRUE", &tmp_ac_prefs.encrypt_reply, P_BOOL},
	{"ascii_armored", "FALSE", &tmp_ac_prefs.ascii_armored, P_BOOL},
	{"clearsign", "FALSE", &tmp_ac_prefs.clearsign, P_BOOL},
	{"sign_key", NULL, &tmp_ac_prefs.sign_key, P_ENUM},
	{"sign_key_id", NULL, &tmp_ac_prefs.sign_key_id, P_STRING},

	/* SSL */
	{"ssl_pop", "0", &tmp_ac_prefs.ssl_pop, P_ENUM},
	{"ssl_imap", "0", &tmp_ac_prefs.ssl_imap, P_ENUM},
	{"ssl_nntp", "0", &tmp_ac_prefs.ssl_nntp, P_ENUM},
	{"ssl_smtp", "0", &tmp_ac_prefs.ssl_smtp, P_ENUM},
	{"use_nonblocking_ssl", "1", &tmp_ac_prefs.use_nonblocking_ssl, P_BOOL},

	/* Advanced */
	{"set_smtpport", "FALSE", &tmp_ac_prefs.set_smtpport, P_BOOL},
	{"smtp_port", "25", &tmp_ac_prefs.smtpport, P_USHORT},
	{"set_popport", "FALSE", &tmp_ac_prefs.set_popport, P_BOOL},
	{"pop_port", "110", &tmp_ac_prefs.popport, P_USHORT},
	{"set_imapport", "FALSE", &tmp_ac_prefs.set_imapport, P_BOOL},
	{"imap_port", "143", &tmp_ac_prefs.imapport, P_USHORT},
	{"set_nntpport", "FALSE", &tmp_ac_prefs.set_nntpport, P_BOOL},
	{"nntp_port", "119", &tmp_ac_prefs.nntpport, P_USHORT},
	{"set_domain", "FALSE", &tmp_ac_prefs.set_domain, P_BOOL},
	{"domain", NULL, &tmp_ac_prefs.domain, P_STRING},
	{"imap_directory", NULL, &tmp_ac_prefs.imap_dir, P_STRING},
	{"imap_clear_cache_on_exit", "FALSE",
	 &tmp_ac_prefs.imap_clear_cache_on_exit, P_BOOL},
	{"set_sent_folder", "FALSE", &tmp_ac_prefs.set_sent_folder, P_BOOL},
	{"sent_folder", NULL, &tmp_ac_prefs.sent_folder, P_STRING},
	{"set_draft_folder", "FALSE", &tmp_ac_prefs.set_draft_folder, P_BOOL},
	{"draft_folder", NULL, &tmp_ac_prefs.draft_folder, P_STRING},
	{"set_queue_folder", "FALSE", &tmp_ac_prefs.set_queue_folder, P_BOOL},
	{"queue_folder", NULL, &tmp_ac_prefs.queue_folder, P_STRING},
	{"set_trash_folder", "FALSE", &tmp_ac_prefs.set_trash_folder, P_BOOL},
	{"trash_folder", NULL, &tmp_ac_prefs.trash_folder, P_STRING},

	{NULL, NULL, NULL, P_OTHER}
};

static gint prefs_account_get_new_id(void);


PrefsAccount *prefs_account_get_tmp_prefs(void)
{
	return &tmp_ac_prefs;
}

void prefs_account_set_tmp_prefs(PrefsAccount *ac_prefs)
{
	tmp_ac_prefs = *ac_prefs;
}

void prefs_account_apply_tmp_prefs(PrefsAccount *ac_prefs)
{
	*ac_prefs = tmp_ac_prefs;
}

PrefParam *prefs_account_get_params(void)
{
	return param;
}

PrefsAccount *prefs_account_new(void)
{
	PrefsAccount *ac_prefs;

	ac_prefs = g_new0(PrefsAccount, 1);
	memset(&tmp_ac_prefs, 0, sizeof(PrefsAccount));
	prefs_set_default(param);
	*ac_prefs = tmp_ac_prefs;
	ac_prefs->account_id = prefs_account_get_new_id();

	return ac_prefs;
}

void prefs_account_read_config(PrefsAccount *ac_prefs, const gchar *label)
{
	const gchar *p = label;
	gchar *rcpath;
	gint id;

	g_return_if_fail(ac_prefs != NULL);
	g_return_if_fail(label != NULL);

	memset(&tmp_ac_prefs, 0, sizeof(PrefsAccount));

	rcpath = g_strconcat(get_rc_dir(), G_DIR_SEPARATOR_S, ACCOUNT_RC, NULL);
	prefs_read_config(param, label, rcpath, NULL);
	g_free(rcpath);

	*ac_prefs = tmp_ac_prefs;
	while (*p && !g_ascii_isdigit(*p)) p++;
	id = atoi(p);
	if (id < 0) g_warning("wrong account id: %d\n", id);
	ac_prefs->account_id = id;

	if (ac_prefs->protocol == A_APOP) {
		debug_print("converting protocol A_APOP to new prefs.\n");
		ac_prefs->protocol = A_POP3;
		ac_prefs->use_apop_auth = TRUE;
	}

	custom_header_read_config(ac_prefs);
}

void prefs_account_write_config_all(GList *account_list)
{
	GList *cur;
	gchar *rcpath;
	PrefFile *pfile;

	rcpath = g_strconcat(get_rc_dir(), G_DIR_SEPARATOR_S, ACCOUNT_RC, NULL);
	if ((pfile = prefs_file_open(rcpath)) == NULL) {
		g_free(rcpath);
		return;
	}
	g_free(rcpath);

	for (cur = account_list; cur != NULL; cur = cur->next) {
		tmp_ac_prefs = *(PrefsAccount *)cur->data;
		if (fprintf(pfile->fp, "[Account: %d]\n",
			    tmp_ac_prefs.account_id) <= 0 ||
		    prefs_file_write_param(pfile, param) < 0) {
			g_warning(_("failed to write configuration to file\n"));
			prefs_file_close_revert(pfile);
			return;
		}
		if (cur->next) {
			if (fputc('\n', pfile->fp) == EOF) {
				FILE_OP_ERROR(rcpath, "fputc");
				prefs_file_close_revert(pfile);
				return;
			}
		}
	}

	if (prefs_file_close(pfile) < 0)
		g_warning(_("failed to write configuration to file\n"));
}

void prefs_account_free(PrefsAccount *ac_prefs)
{
	if (!ac_prefs) return;

	tmp_ac_prefs = *ac_prefs;
	prefs_free(param);

	if (ac_prefs->tmp_pass)
		g_free(ac_prefs->tmp_pass);
	g_free(ac_prefs);
}

static gint prefs_account_get_new_id(void)
{
	GList *ac_list;
	PrefsAccount *ac;
	static gint last_id = 0;

	for (ac_list = account_get_list(); ac_list != NULL;
	     ac_list = ac_list->next) {
		ac = (PrefsAccount *)ac_list->data;
		if (last_id < ac->account_id)
			last_id = ac->account_id;
	}

	return last_id + 1;
}
