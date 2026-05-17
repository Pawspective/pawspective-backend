#pragma once

#include "../../dto/include/post_create_dto.hpp"
#include "../../dto/include/post_dto.hpp"
#include "../../dto/include/post_list_dto.hpp"
#include "../../dto/include/post_update_dto.hpp"
#include "../../repositories/include/post_repository.hpp"
#include "organization_service.hpp"
#include "services/user_service.hpp"

namespace pawspective::services {

class PostService {
public:
    explicit PostService(
        const repositories::PostRepository& repo,
        const services::OrganizationService& org_service,
        const services::UserService& user_service
    );
    dto::PostDTO Create(int64_t user_id, const dto::PostCreateDTO& dto) const;
    [[nodiscard]] dto::PostDTO Get(int64_t id) const;
    dto::PostDTO Update(int64_t user_id, int64_t post_id, const dto::PostUpdateDTO& dto) const;
    [[nodiscard]] dto::PostListDTO GetByOrganizationPaginated(int64_t org_id, int page) const;
    void Delete(int64_t user_id, int64_t post_id) const;

private:
    const repositories::PostRepository& repository_;
    const services::UserService& user_service_;
    const services::OrganizationService& org_service_;
};

}  // namespace pawspective::services
