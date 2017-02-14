function [subont] = depth_n_subont(ont, d)
    %DEPTH_N_SUBONT
    %
    %   [subont] = DEPTH_N_SUBONT(ont, d);
    %
    %       Creates a sub-ontology contain terms up to a specific depth.
    %
    % Input
    % -----
    % [struct]
    % ont:  The ontology structure, see pfp_ontbuild.m.
    %
    % [double]
    % d:    A depth.
    %
    % Output
    % ------
    % [struct]
    % subont:   The resulting sub-ontology.
    %
    % See also
    % --------
    % [>] pfp_ontbuild.m
    %
    % Dependency
    % ----------
    % [>] pfp_depth.m
    % [>] pfp_ancestorterm.m
    % [>] pfp_subont.m

    % check inputs {{{
    if nargin ~= 2
        error('depth_n_subont:InputCount', 'Expected 2 inputs.');
    end

    % ont
    validateattributes(ont, {'struct'}, {'nonempty'}, '', 'ont', 1);

    % d
    validateattributes(d, {'double'}, {'positive', 'integer'}, '', 'd', 2);
    % }}}

    depth = pfp_depth(ont, ont.term);
    subont = pfp_subont(ont, pfp_ancestorterm(ont, ont.term(depth<=d)));
end

% -------------
% Yuxiang Jiang (yuxjiang@indiana.edu)
% Department of Computer Science
% Indiana University, Bloomington
% Last modified: Sun 12 Feb 2017 12:33:05 AM E
