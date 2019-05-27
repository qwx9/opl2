#include <u.h>
#include <libc.h>
#include <bio.h>

uchar*	opl2out(uchar *, int);
void	opl2wr(int, int);
void	opl2init(int);

enum{
	Rate = 44100,
};

void
usage(void)
{
	fprint(2, "usage: %s [-n nsamp] [file]\n", argv0);
	exits("usage");
}

void
main(int argc, char **argv)
{
	int r, v, dt, nsamp, fd;
	uchar *sb, u[4];
	Biobuf *bi, *bo;

	fd = 0;
	nsamp = Rate / 700;
	ARGBEGIN{
	case 'n':
		nsamp = Rate / atoi(EARGF(usage()));
		break;
	default:
		usage();
	}ARGEND;
	if(*argv != nil)
		if((fd = open(*argv, OREAD)) < 0)
			sysfatal("open: %r");
	bi = Bfdopen(fd, OREAD);
	bo = Bfdopen(1, OWRITE);
	if(bi == nil || bo == nil)
		sysfatal("Bfdopen: %r");
	nsamp *= 4;
	if((sb = malloc(nsamp)) == nil)
		sysfatal("malloc: %r");
	opl2init(Rate);
	while(Bread(bi, u, sizeof u) > 0){
		r = u[0];
		v = u[1];
		dt = u[3]<<8 | u[2];
		opl2wr(r, v);
		while(dt-- > 0){
			opl2out(sb, nsamp);
			Bwrite(bo, sb, nsamp);
		}
	}
}
