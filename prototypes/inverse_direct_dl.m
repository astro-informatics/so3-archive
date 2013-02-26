function [ f ] = inverse_direct_dl( L, flm )
%INVERSE_DIRECT Inverse spherical harmonic transform using naive approach
%   This computes the inverse spherical harmonic transform, simply
%   synthesising the function by summing over all spherical harmonics.
%
%   L   ... the band limit (maximum l is L-1)
%   flm ... the coefficients of the spherical harmonics
%           to avoid wasting memory, these should be supplied in an
%           unrolled array of the following format:
%           [(0,0) (1,-1) (1,0) (1,1) (2,-2) (2,-1) ... ]
%           of size L^2, where the first number corresponds to l
%           and the second to m.

if length(flm) ~= L^2
    error('Parameter flm has to contain L^2 coefficients.')
end

[thetas, phis] = ssht_sampling(L);

f = zeros(length(thetas), length(phis));

for j=1:length(thetas),
    theta = thetas(j);
    dl = zeros(2*L-1,2*L-1,L);
    dl(:,:,1) = ssht_dl(dl(:,:,1), L, 0, theta);
    for l = 1:L-1,
        dl(:,:,l+1) = ssht_dl(dl(:,:,l), L, l, theta);
    end
    for k=1:length(phis),
        phi = phis(k);
        sum = 0;
        lmindex = 1;
        for l = 0:L-1,
            for m = -l:l,
                sum = sum + flm(lmindex)*sqrt((2*l+1)/(4*pi))*dl(m+L, L, l+1)*exp(1i*m*phi);
                lmindex = lmindex + 1;
            end
        end
        f(j,k) = sum;
    end
end