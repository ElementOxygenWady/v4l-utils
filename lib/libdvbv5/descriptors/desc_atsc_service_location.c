/*
 * Copyright (c) 2013 - Mauro Carvalho Chehab <mchehab@kernel.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation version 2.1 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 */

#include <libdvbv5/desc_atsc_service_location.h>
#include <libdvbv5/dvb-fe.h>
#include <ctype.h>

#if __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif

int atsc_desc_service_location_init(struct dvb_v5_fe_parms *parms,
				     const uint8_t *buf, struct dvb_desc *desc)
{
	struct atsc_desc_service_location *s_loc = (struct atsc_desc_service_location *)desc;
	struct atsc_desc_service_location_elementary *el;
	unsigned char *p = (unsigned char *)buf;
	int i;
	size_t len, dlen = desc->length;

	// raw data should have one element
	len = sizeof(u_int16_t) + sizeof(u_int8_t) + sizeof(struct atsc_desc_service_location_elementary);
	if (dlen < len) {
		dvb_logwarn("ATSC service location descriptor is too small");
		return -1;
	}

	memcpy(&s_loc->bitfield , p, sizeof(u_int16_t));
	p += sizeof(u_int16_t);
	dlen -= sizeof(u_int16_t);
	bswap16(s_loc->bitfield);

	memcpy(&s_loc->number_elements , p, sizeof(u_int8_t));
	p += sizeof(u_int8_t);
	dlen -= sizeof(u_int8_t);

	bswap16(s_loc->bitfield);

	len = s_loc->number_elements * sizeof(*s_loc->elementary);
	if (dlen < len) {
		dvb_logwarn("ATSC service location descriptor is too small for %d elements",
			    s_loc->number_elements);
		return -1;
	}
	if (dlen > len) {
		dvb_logwarn("ATSC service location descriptor %zu bytes bigger than expected",
			    dlen - len);
		return -1;
	}

	if (s_loc->number_elements) {
		s_loc->elementary = malloc(len);
		if (!s_loc->elementary) {
			dvb_perror("Can't allocate space for ATSC service location elementary data");
			return -1;
		}

		el = s_loc->elementary;

		for (i = 0; i < s_loc->number_elements; i++) {
			memcpy(el, p, sizeof(*el));
			bswap16(el->bitfield);

			el++;
			p += sizeof(*el);
		}
	} else {
		s_loc->elementary = NULL;
	}
	return 0;
}

#define prt_char(x) (isprint(x) ? x : '.')

void atsc_desc_service_location_print(struct dvb_v5_fe_parms *parms, const struct dvb_desc *desc)
{
	const struct atsc_desc_service_location *s_loc = (const struct atsc_desc_service_location *) desc;
	struct atsc_desc_service_location_elementary *el = s_loc->elementary;
	int i;

	dvb_loginfo("|           pcr PID               %d", s_loc->pcr_pid);
	dvb_loginfo("|\\ elementary service - %d elementaries", s_loc->number_elements);
	for (i = 0; i < s_loc->number_elements; i++) {
		dvb_loginfo("|-  elementary %d", i);
		dvb_loginfo("|-      | stream type 0x%02x", el[i].stream_type);
		dvb_loginfo("|-      | PID         %d", el[i].elementary_pid);
		dvb_loginfo("|-      | Language    %c%c%c (0x%02x 0x%02x 0x%02x)",
			    prt_char(el[i].ISO_639_language_code[0]),
			    prt_char(el[i].ISO_639_language_code[1]),
			    prt_char(el[i].ISO_639_language_code[2]),
			    el[i].ISO_639_language_code[0],
			    el[i].ISO_639_language_code[1],
			    el[i].ISO_639_language_code[2]);
	}
}

void atsc_desc_service_location_free(struct dvb_desc *desc)
{
	const struct atsc_desc_service_location *s_loc = (const struct atsc_desc_service_location *) desc;

	if (s_loc->elementary)
		free(s_loc->elementary);
}
